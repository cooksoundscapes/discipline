import subprocess
import re
import logging
from pprint import pprint
from enum import Enum
import os

class PortType(Enum):
    NONE = 0
    INPUT = 1
    OUTPUT = 2

class Port():
    def __init__(self):
        self.connections = []
        self.type = PortType.NONE

    def __repr__(self):
        return f'{{ type: {self.type}, connections: {str(self.connections)} }}'

class Client():
    def __init__(self, ports):
        self.ports = ports

    def __repr__(self):
        return f'{{ ports: {str(self.ports)} }}'

def get_output(command, check_errors=False, shell=True):
    result = subprocess.run(command, capture_output=True, text=True, shell=shell, check=check_errors)
    return result.stdout

def jack_status():
    status = get_output("jack_control status")
    return int(status.split('\n')[1] == 'started')

def get_jack_clients():
    raw = get_output("jack_lsp -cp")
    lines = raw.splitlines()
    clients = {}
    current_port = None
    for line in lines:
        if not line.startswith(' ') and not line.startswith('\t'):
            *client, port = line.split(":")
            client = ':'.join(client)
            current_port = (client, port)

            if client not in clients:
                clients[client] = Client(ports={port: Port()})

            elif port not in clients[client].ports:
                clients[client].ports[port] = Port()

        elif line.startswith('   '):
            clients[current_port[0]].ports[current_port[1]].connections.append(line.strip())

        elif line.startswith('\t'):
            port_type = re.search(r'^properties: (.*?),', line.strip())
            if port_type:
                port_type = PortType[port_type.group(1).upper()]
                clients[current_port[0]].ports[current_port[1]].type = port_type
    #pprint(clients)
    return clients

def get_connection_graph():
    clients = get_jack_clients()
    graph = ""
    for c_name, client in clients.items():
        for p_name, port in client.ports.items():
            if port.type == PortType.OUTPUT:
                for conn in port.connections:
                    graph += f'{c_name}:{p_name} -> {conn}\n'
    return graph

def connect(src, dest):
    get_output(f'jack_connect {src} {dest}', check_errors=True)

def disconnect(src, dest):
    get_output(f'jack_disconnect {src} {dest}', check_errors=True)

def start_jack(config):
    get_output(f'''
    jack_control stop &&
    jack_control dps device hw:{config['card']} &&
    jack_control dps rate {config['rate']} &&
    jack_control dps period {config['period']} &&
    jack_control dps nperiods {config['nperiod']} &&
    jack_control dps inchannels {config['inchan']} &&
    jack_control dps outchannels {config['outchan']} &&
    jack_control start
    ''', check_errors=True)

def stop_jack():
    get_output(f'jack_control stop', check_errors=True)

def get_sound_cards():
    raw = get_output("cat /proc/asound/cards")
    pat = r'\[\s*(\w+)\s*\]'
    return re.findall(pat, raw)

def get_current_config():
    raw = get_output("jack_control dp")
    result = re.findall(r'^\s*(?:device|rate|period):.*?:(\w*?)\)$', raw, re.MULTILINE)
    return f'hw:{result[0]} - {result[1]}Hz - b.size:{result[2]}'

def manage_pd(verb):
    return get_output(f'systemctl --user {verb} pd')

def pd_start():
    manage_pd("start")

def pd_restart():
    manage_pd("restart")

def pd_stop():
    manage_pd("stop")

def pd_status():
    s = manage_pd("status")
    s = re.search(r'^\s*Active:\s*(.*?)\s*\(', s, re.MULTILINE)
    return int(s.group(1) == 'active')

def system_reboot():
    os.system("sudo reboot")

def system_poweroff():
    os.system("sudo poweroff")