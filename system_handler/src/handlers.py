from pythonosc.udp_client import SimpleUDPClient
from system_utils import *
from time import sleep

'''
UI:
* CPU load
* JACk status
* static IP
pd status
jack change preset:
    filtra a lista de presets só pelos cards que existem em self.sound_cards;
    ultima opção é um "other" e aí vc pode escolher um card que nao tem preset
pd settings:
    restart DSP
    change main patch - como??
system:
    reboot
    shutdown
'''

class AppState():
    def __init__(self):
        self.ui_client = SimpleUDPClient("localhost", 7777)
        self.at_home_screen = True
        self.jack_presets = {
            "internal": {"card": "Generic", "rate": 48000, "period": 256, "nperiod": 2, "inchan": 2, "outchan": 2},
            "Zoom U22": {"card": "U22", "rate": 48000, "period": 128, "nperiod": 3, "inchan": 2, "outchan": 2},
            "UMC404": {"card": "U192k", "rate": 48000, "period": 128, "nperiod": 3, "inchan": 4, "outchan": 4},
            "UMC404 96kHz": {"card": "U192k", "rate": 96000, "period": 128, "nperiod": 3, "inchan": 4, "outchan": 4}
        }
        self.jack_clients = get_jack_clients()
        self.sound_cards = get_sound_cards()

    def watchdog(self):
        try:
            while True:
                if self.at_home_screen:
                    status = jack_status()
                    self.ui_client.send_message("/param", ["jack-status", status])
                sleep(3)
        except KeyboardInterrupt:
            pass

app_state = AppState()

def encoders(addr, *args):
    print(addr, args)

def seq_buttons(addr, *args):
    print(addr, args)

def nav_buttons(addr, *args):
    print(addr, args)
