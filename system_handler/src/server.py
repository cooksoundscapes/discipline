from pythonosc.osc_server import BlockingOSCUDPServer
from pythonosc.dispatcher import Dispatcher
from threading import Thread

class Server():
    def __init__(self, host, port, endpoints):
        self._dispatcher = Dispatcher()
        for path, action in endpoints:
            self._dispatcher.map(path, action)
        self._host = host
        self._port = port
        self._server = BlockingOSCUDPServer(
            (self._host, self._port),
            self._dispatcher)

    def start(self):
        try:
            print(f'Listening on {self._host}:{self._port}')
            self._server.serve_forever()
        except KeyboardInterrupt:
            pass
        finally:
            self.stop()

    def stop(self):
        self._server.shutdown()

class ServerThread(Server):
    def __init__(self, host, port, endpoints):
        super().__init__(host, port, endpoints)

    def __enter__(self):
        self._thread = Thread(target=super().start)
        self._thread.start()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        super().stop() 
        self._thread.join()
