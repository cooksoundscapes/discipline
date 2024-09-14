from pythonosc.udp_client import SimpleUDPClient
from jack_utils import *
from time import sleep

class AppState():
    def __init__(self):
        self.ui_client = SimpleUDPClient("localhost", 7777)
        self.at_home_screen = True
        self.jack_presets = [
            {"name": "internal", "card": "Generic", "rate": 48000, "period": 256, "nperiod": 2, "inchan": 2, "outchan": 2},
            {"name": "Zoom U22", "card": "U22", "rate": 48000, "period": 128, "nperiod": 3, "inchan": 2, "outchan": 2},
            {"name": "UMC404", "card": "U192k", "rate": 48000, "period": 128, "nperiod": 3, "inchan": 4, "outchan": 4},
            {"name": "UMC404 96kHz", "card": "Generic", "rate": 96000, "period": 128, "nperiod": 3, "inchan": 4, "outchan": 4}
        ]
        self.jack_clients = []

    def watchdog(self):
        try:
            while True:
                if self.at_home_screen:
                    status = jack_status()
                    self.ui_client.send_message("/param", ["jack-status", status])
                sleep(3)
        except KeyboardInterrupt:
            pass

    def check_pd_service():
        pass

    def retrieve_sys_info(self):
        jack_clients = get_jack_clients()
        get_sound_cards()
        

app_state = AppState()

def encoders(addr, *args):
    print(addr, args)

def seq_buttons(addr, *args):
    print(addr, args)

def nav_buttons(addr, *args):
    print(addr, args)
