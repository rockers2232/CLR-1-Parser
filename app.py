from flask import Flask, request, jsonify, render_template
import subprocess
import os

app = Flask(__name__, template_folder='.')

@app.route('/')
def index():
    return render_template('index.html')

def parse_cpp_output(raw_text):
    data = {'grammar': [], 'first_follow': [], 'action': [], 'goto': [], 'trace': [], 'status': '', 'error': None}
    current_section = None
    
    for line in raw_text.split('\n'):
        line = line.strip()
        if not line: continue
        
        if line.startswith('@'):
            current_section = line[1:]
            continue
            
        if current_section == 'GRAMMAR':
            parts = line.split('|')
            if len(parts) == 3:
                data['grammar'].append({'rule': parts[0], 'lhs': parts[1], 'rhs': parts[2]})
        elif current_section == 'FIRST_FOLLOW':
            parts = line.split('|')
            if len(parts) == 3:
                data['first_follow'].append({'nt': parts[0], 'first': parts[1], 'follow': parts[2]})
        elif current_section == 'ACTION_TABLE':
            parts = line.split('|')
            if len(parts) == 3:
                data['action'].append({'state': parts[0], 'sym': parts[1], 'act': parts[2]})
        elif current_section == 'GOTO_TABLE':
            parts = line.split('|')
            if len(parts) == 3:
                data['goto'].append({'state': parts[0], 'nt': parts[1], 'go': parts[2]})
        elif current_section == 'TRACE':
            parts = line.split('|')
            if len(parts) == 3:
                data['trace'].append({'stack': parts[0], 'input': parts[1], 'action': parts[2]})
        elif current_section == 'STATUS':
            data['status'] = line
        elif current_section == 'ERROR':
            data['error'] = line
            
    return data

@app.route('/parse', methods=['POST'])
def parse():
    data = request.json
    code = data.get('code', '')

    with open('temp_source.txt', 'w') as f:
        f.write(code)

    try:
        if not os.path.exists('parser_engine') and not os.path.exists('parser_engine.exe'):
            compile_process = subprocess.run(
                ['g++', 'parser.cpp', '-o', 'parser_engine'],
                capture_output=True, text=True
            )
            if compile_process.returncode != 0:
                return jsonify({"error": "Compilation Error:\n" + compile_process.stderr})

        executable = './parser_engine' if os.name != 'nt' else 'parser_engine.exe'
        
        with open('temp_source.txt', 'r') as infile:
            process = subprocess.run([executable], stdin=infile, capture_output=True, text=True, timeout=5)

        parsed_data = parse_cpp_output(process.stdout)
        return jsonify(parsed_data)

    except Exception as e:
        return jsonify({"error": str(e)})

if __name__ == '__main__':
    app.run(debug=True, port=5000)