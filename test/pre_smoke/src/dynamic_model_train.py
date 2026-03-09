# Copyright 2026 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import os
import torch
import multiprocessing as mp
import time
import socket
import torch_npu
import torch.nn as nn
import torch.optim as optim
import torch.distributed as dist
from torch.nn.parallel import DistributedDataParallel as DDP

def get_local_ip():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
        s.close()
        return ip
    except Exception:
        return '127.0.0.1'

class AllReduceTrainingModel(nn.Module):
    def __init__(self, input_size=5, hidden_size=10, output_size=5):
        super(AllReduceTrainingModel, self).__init__()
        self.network = nn.Sequential(
            nn.Linear(input_size, hidden_size),
            nn.ReLU(),
            nn.Linear(hidden_size, output_size)
        )

    def forward(self, x):
        return self.network(x)

    def get_training_data(self, rank, batch_size=10):
        inputs = torch.ones(batch_size, 5) * (rank + 1)
        labels = torch.ones(batch_size, 5) * (2 - rank)
        return (inputs.npu() if torch.npu.is_available() else inputs,
                labels.npu() if torch.npu.is_available() else labels)

def worker(rank, world_size=2):
    os.environ['MASTER_ADDR'] = get_local_ip()
    os.environ['MASTER_PORT'] = '55234'
    torch_npu.npu.set_device(rank)
    dist.init_process_group(backend='hccl', world_size=world_size, rank=rank)

    model = AllReduceTrainingModel().npu()
    ddp_model = DDP(model, device_ids=[rank])
    optimizer = optim.SGD(ddp_model.parameters(), lr=0.001)
    criterion = nn.MSELoss()
    input_data, labels = model.get_training_data(rank)
    print(f"Rank {rank} before training: input mean = {input_data.mean().cpu().item()}")

    for j in range(30):
        time.sleep(1)
        optimizer.zero_grad()
        outputs = ddp_model(input_data)
        loss = criterion(outputs, labels)
        loss.backward()
        optimizer.step()
        print(f"Rank {rank} step {j}, loss: {loss.cpu().item()}")
    dist.destroy_process_group()

def test_allreduce_profiler_with_training():
    processes = []
    ctx = mp.get_context('spawn')
    for i in range(2):
        p = ctx.Process(target=worker, args=(i,))
        p.start()
        processes.append(p)

    for p in processes:
        p.join()

if __name__ == '__main__':
    test_allreduce_profiler_with_training()
    print("model train over...")
