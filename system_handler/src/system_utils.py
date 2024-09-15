import subprocess
import re
import logging
from pprint import pprint
from enum import Enum

class PortType(Enum):
    NONE = 0,
    INPUT = 1,
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

def get_output(command, check_errors=False):
    result = subprocess.run(command, capture_output=True, text=True, shell=True, check=check_errors)
    return result.stdout

def jack_status():
    status = get_output("jack_control status")
    return status.split('\n')[1]

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

def get_IPs():
    pass