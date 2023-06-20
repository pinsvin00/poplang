import os
import shutil
import subprocess
from threading import Thread
from multiprocessing import Pool, Lock
import sys

failed_count = 0
good_count = 0

lock = Lock()
def atomic_print(msg):
    msg = '%s%s' % (msg, os.linesep)
    with lock:
        sys.stdout.write(msg)



def run_and_check_output(cmd, res_file):
    global good_count
    global failed_count
    try:
        with open(res_file) as output_file:
            expected_output = output_file.read()
            proc = subprocess.run(cmd, capture_output=True, text=True)
            res = proc.stdout
            if expected_output == res:
                atomic_print(f'{cmd} - OK')
                good_count += 1
            else:
                atomic_print(f'{cmd} - FAIL')
                failed_count += 1
    except:
        atomic_print(f"Failed to run command {cmd}")


def run_tests():
    output_files, code_files = test_files()

    if sys.platform == "win32":
        executable_file = "popdb.exe"
    else:
        executable_file = "popdb"

    if os.path.isfile(executable_file):
        os.remove(executable_file)
    try:
        shutil.copyfile(f"../build/{executable_file}", executable_file)
    except RuntimeError as ex:
         atomic_print(ex)
         return
    
    threads = []
    for code_file in code_files:
        expected_output_dir = code_file.replace(".pop", ".txt")
        if expected_output_dir in output_files:
            if sys.platform == "win32":
                thread = Thread(target=run_and_check_output, args=[f'{executable_file} {code_file}', expected_output_dir])
            else:
                thread = Thread(target=run_and_check_output, args=[f'./{executable_file} {code_file}', expected_output_dir])
            thread.start()
            threads.append(thread)

    for t in threads:
        t.join()

    print(f'Successful tests {good_count}/{good_count + failed_count}')
    

def test_files():
    dirs = os.listdir()
    output_files = []
    code_files = []

    for dir in dirs:
        if dir.endswith(".txt"):
            output_files.append(dir)
        elif dir.endswith(".pop"):
            code_files.append(dir)

    return output_files, code_files
    
    

if __name__ == "__main__":
    run_tests()
