import subprocess
import re
from pprint import pprint

class Port():
    def __init__(self, connections);
        self.connections = connections

class Client():
    def __init__(self, ports):
        self.ports = ports

def get_output(command):
    result = subprocess.run(command, capture_output=True, text=True, shell=True)
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
                clients[client] = {
                    'ports': {
                        port: {
                            'connected_to': []
                        }
                    }
                }
            elif port not in clients[client]['ports']:
                clients[client]['ports'][port] = {
                    'connected_to': []
                }
        elif line.startswith('   '):
            clients[current_port[0]]['ports'][current_port[1]]['connected_to'].append(line.strip())
        elif line.startswith('\t'):
            port_type = re.search(r'^properties: (.*?),', line.strip())
            if port_type:
                clients[current_port[0]]['ports'][current_port[1]]['type'] = port_type.group(1)
    return clients

def connect(src, dest):
    pass

def disconnect(src, dest):
    pass

def start_jack(config):
    pass

def stop_jack():
    pass

def get_sound_cards():
    raw = get_output("cat /proc/asound/cards")
    pat = r'\[\s*(\w+)\s*\]'
    return re.findall(pat, raw)

