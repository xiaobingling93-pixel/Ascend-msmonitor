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
import logging
import sqlite3

logging.basicConfig(level=logging.INFO,
                    format='\n%(asctime)s %(filename)s [line:%(lineno)d] [%(levelname)s] %(message)s')

class EmptyClass:
    def __init__(self: any, info: str = "") -> None:
        self._info = info

    @classmethod
    def __bool__(cls: any) -> bool:
        return False

    @classmethod
    def __str__(cls: any) -> str:
        return ""

    @property
    def info(self: any) -> str:
        return self._info

    @staticmethod
    def is_empty() -> bool:
        return True

class DBManager:
    FETCH_SIZE = 10000
    INSERT_SIZE = 10000
    TENNSTONS = 10
    NSTOUS = 1000
    MAX_ROW_COUNT = 100000000

    @staticmethod
    def create_connect_db(db_path: str) -> tuple:
        try:
            conn = sqlite3.connect(db_path)
        except sqlite3.Error as err:
            logging.error(str(err), exc_info=False)
            return EmptyClass("empty conn"), EmptyClass("empty curs")

        try:
            if isinstance(conn, sqlite3.Connection):
                curs = conn.cursor()
                os.chmod(db_path, 0o640)
                return conn, curs
        except sqlite3.Error:
            logging.info("conn sqlite error")
            return EmptyClass("empty conn"), EmptyClass("empty curs")
        return EmptyClass("empty conn"), EmptyClass("empty curs")

    @staticmethod
    def destroy_db_connect(conn: any, cur: any) -> None:
        try:
            if isinstance(cur, sqlite3.Cursor):
                cur.close()
        except sqlite3.Error as error:
            logging.error(str(error), exc_info=False)

        try:
            if isinstance(conn, sqlite3.Connection):
                conn.close()
        except sqlite3.Error as error:
            logging.error(str(error), exc_info=False)

    @classmethod
    def fetch_all_data(cls: any, curs: any, sql: str, param: tuple = None) -> list:
        """
        fetch 10000 num of data each time to get all data
        """
        if not isinstance(curs, sqlite3.Cursor):
            return []
        data = []
        try:
            if param:
                res = curs.execute(sql, param)
            else:
                res = curs.execute(sql)
        except sqlite3.Error as _err:
            logging.error("%s", str(_err), exc_info=False)
            logging.debug("%s, sql: %s", str(_err), sql, exc_info=False)
            curs.row_factory = None
            return []
        try:
            while True:
                res = curs.fetchmany(cls.FETCH_SIZE)
                data += res
                if len(data) > cls.MAX_ROW_COUNT:
                    logging.error("Please check the record counts in %s's table",
                                  os.path.basename(curs.execute("PRAGMA database_list;").fetchone()[-1]))

                if len(res) < cls.FETCH_SIZE:
                    break
            return data
        except sqlite3.Error as _err:
            logging.error(str(_err), exc_info=False)
            return []
        finally:
            curs.row_factory = None

    @classmethod
    def fetch_all_field_name_in_table(cls: any, db_path: str, table_name: str) -> list:
        conn, curs = DBManager.create_connect_db(db_path)
        if not (conn and curs):
            return []

        sql = f"PRAGMA table_info({table_name})"
        try:
            data = DBManager.fetch_all_data(curs, sql)
            return [info[1] for info in data]
        except sqlite3.Error as _err:
            raise RuntimeError(f"Failed to fetch data, ERROR: {_err}")
        finally:
            DBManager.destroy_db_connect(conn, curs)
