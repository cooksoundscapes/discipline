from server import Server
from handlers import app_state
import handlers

def main():
    app_state.retrieve_sys_info()

    endpoints = [
        ("/encoders/*", handlers.encoders),
        ("/seq-buttons/*", handlers.seq_buttons),
        ("/nav-buttons/*", handlers.nav_buttons),
    ]
    server = Server('localhost', 9001, endpoints)
    server.start()

if __name__ == "__main__":
    main()