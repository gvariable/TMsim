import random
import subprocess

prefix = "./bin/turing ./programs/case1.tm "

total, num = 200, 0

for i in range(total):
    a = "a"*random.randint(1, 100)
    b = "b"*random.randint(1, 100)
    command = prefix + a + b
    result = subprocess.run(command.split(), stdout=subprocess.PIPE)
    accepted, retval = result.stdout.decode("utf-8").split()
    if accepted == "(ACCEPTED)":
        if retval != 'c'*(len(a) * len(b)):
            print("Test case failed: ", command, " -> ", retval)
            num += 1
    elif accepted == "(UNACCEPTED)":
        print("Test case failed: ", command, " -> ", retval)
        num += 1
print(f"Test cases pass: {total - num}/{total}")

num = 0
corners_fail = ["a", "aa", "abbbba", "aba", "b", "bbbaa", "baa"]
for case in corners_fail:
    command = prefix + case
    result = subprocess.run(command.split(), stdout=subprocess.PIPE)
    accepted, retval = result.stdout.decode("utf-8").split()
    if accepted == "(UNACCEPTED)" and retval != "Illegal_Input":
        print("Test case failed: ", command, " -> ", retval)
        num += 1
    if accepted == "(ACCEPTED)":
        print("Test case failed: ", command, " -> ", retval)
        num += 1

print(f"Test cases pass: {len(corners_fail) - num}/{len(corners_fail)}")
