from pythonosc.udp_client import SimpleUDPClient
from system_utils import *
from time import sleep
import logging
from enum import Enum
import psutil

class MenuOption(Enum):
    JACK_PRESET = 0
    SYSTEM = 1

class SystemOption(Enum):
    RESTART_PD = 0
    REBOOT = 1
    SHUTDOWN = 2

class App():
    def __init__(self):
        self.ui_client = SimpleUDPClient("127.0.0.1", 7777)
        self.at_home_screen = True
        self.main_project_ui = "fx_stack" #where it navigates when leaving home
        self.jack_presets = {
            "internal": {"card": "Generic", "rate": 48000, "period": 256, "nperiod": 2, "inchan": 2, "outchan": 2},
            "Zoom U22": {"card": "U22", "rate": 48000, "period": 128, "nperiod": 3, "inchan": 2, "outchan": 2},
            "UMC404": {"card": "U192k", "rate": 48000, "period": 128, "nperiod": 3, "inchan": 4, "outchan": 4},
            "UMC404 96kHz": {"card": "U192k", "rate": 96000, "period": 128, "nperiod": 3, "inchan": 4, "outchan": 4}
        }
        self.current_preset = 0
        self.ui = {
            "menu-select": 0,
            "submenu-select": 0,
            "jack-status": 0,
            "pd-status": 0,
            "jack-current-settings": "",
            "jack-presets": []
        }
        self.refresh()

    def refresh(self):
        try:
            available_sound_cards = get_sound_cards()
            self.update("jack-status", jack_status())
            self.update("pd-status", pd_status())
            self.update("jack-current-settings", get_current_config())
            self.update("jack-presets", "\n".join(
                [p_name for p_name, conf in self.jack_presets.items() if conf['card'] in available_sound_cards]
            ))
        except Exception as e:
            logging.error("Error while fetching system data:", e)

    def send(self, msg):
        self.ui_client.send_message("/param", msg)

    def update(self, param, val):
        self.ui[param] = val
        self.send([param, val])


    def watchdog(self):
        keepalive = True
        while keepalive:
            if self.at_home_screen:
                try:
                    cpu = psutil.cpu_percent(interval=2)
                    mem = psutil.virtual_memory()
                    self.send(["cpu-load", cpu])
                    self.send(["ram-usage", mem.percent])
                except KeyboardInterrupt:
                    keepalive = False
                    print("Booh bye!")

    '''
    Action methods
    '''
    def update_menu(self, incr):
        if not self.at_home_screen:
            return
        v = max(0, min(len(MenuOption) - 1, self.ui['menu-select'] + incr))
        self.update('menu-select', v)
        if v == MenuOption.JACK_PRESET.value:
            self.update('submenu-select', self.current_preset)
        else:
            self.update('submenu-select', 0)

    def update_submenu(self, incr):
        if not self.at_home_screen:
            return
        match self.ui['menu-select']:
            case MenuOption.JACK_PRESET.value:
                limit = len(self.jack_presets.keys()) - 1
                v = max(0, min(limit, self.ui['submenu-select'] + incr))
                self.update('submenu-select', v)
            case MenuOption.SYSTEM.value:
                limit = 2
                v = max(0, min(limit, self.ui['submenu-select'] + incr))
                self.update('submenu-select', v)

    def change_jack_preset(self):
        if not self.at_home_screen:
            return
        if self.ui['menu-select'] == MenuOption.JACK_PRESET.value:
            self.current_preset = int(self.ui['submenu-select'])
            selected = list(self.jack_presets.keys())[self.current_preset]
            print(f'(Re)starting JACK with preset {selected}...')
            try:
                start_jack(self.jack_presets[selected])
                print(f'Audio engine started with preset {selected}')
                self.send(["jack-start-is-error", 0])
            except:
                print(f'Failed to start audio engine with preset {selected}')
                self.send(["jack-start-is-error", 1])
            finally:
                self.update("jack-current-settings", get_current_config())
                self.update("jack-status", jack_status())

    def raise_system_action(self):
        if not self.at_home_screen:
            return
        if self.ui["menu-select"] == MenuOption.SYSTEM.value:
            match self.ui["submenu-select"]:
                case SystemOption.RESTART_PD.value:
                    pd_restart()
                    self.update("pd-status", pd_status())
                case SystemOption.REBOOT.value:
                    system_reboot()
                case SystemOption.SHUTDOWN.value:
                    system_poweroff()

    def process_home_button(self):
        if self.at_home_screen:
            self.ui_client.send_message("/navigate", self.main_project_ui)
            self.at_home_screen = False
        else:
            self.at_home_screen = True
            self.refresh()
            self.ui_client.send_message("/navigate", "home")

app = App()

HOME_BUTTON = 1
BACK_BUTTON = 2
CONFIRM_BUTTON = 3

def get_ctrl(addr):
    return int(addr.split("/")[2])

def encoders(addr, *args):
    if not app.at_home_screen:
        return
    e = get_ctrl(addr)
    if e == 1:
        app.update_menu(args[0])
    elif e == 2:
        app.update_submenu(args[0])

def nav_buttons(addr, *args):
    if not app.at_home_screen:
        return
    btn = get_ctrl(addr)
    if args[0] == 1:
        if btn == CONFIRM_BUTTON:
            match app.ui['menu-select']:
                case MenuOption.JACK_PRESET.value:
                    app.change_jack_preset()
                case MenuOption.SYSTEM.value:
                    app.raise_system_action()
        elif btn == HOME_BUTTON:
            app.process_home_button()


def seq_buttons(addr, *args):
    pass