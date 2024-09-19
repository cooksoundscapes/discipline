from server import ServerThread
import handlers

def main():
    endpoints = [
        ("/encoders/*", handlers.encoders),
        ("/seq-buttons/*", handlers.seq_buttons),
        ("/nav-buttons/*", handlers.nav_buttons),
    ]
    with ServerThread('localhost', 9001, endpoints):
        handlers.app.watchdog()

if __name__ == "__main__":
    main()