import random
import subprocess

prefix = "./bin/turing ./programs/case2.tm "

total, num = 200, 0

for i in range(total):
    s = "".join(random.choices("ab", k=random.randint(1, 100)))
    s = s + "c" + s
    command = prefix + s

    result = subprocess.run(command.split(), stdout=subprocess.PIPE)
    accepted, retval = result.stdout.decode("utf-8").split()

    if accepted == "(ACCEPTED)":
        if retval != "true":
            print("Test case failed: ", command, " -> ", retval)
            num += 1
print("String in language pass cases: {}/{}".format(total - num, total))


corners_pass = ["c", "bababcbabab"]
corners_fail = ["ac", "bc", "ca", "cb", "acca", "accca", "cac", "cc", "ccc", "acbacb",
                "abcabc", "aacbb", "abccba", "cbaabc", "acbac", "bcaacb", "abcaabb", "baabcab"]
for case in corners_pass:
    command = prefix + case
    result = subprocess.run(command.split(), stdout=subprocess.PIPE)

    accepted, retval = result.stdout.decode("utf-8").split()

    if accepted == "(ACCEPTED)":
        if retval != "true":
            print("Test case failed: ", command, " -> ", retval)
            num += 1
print("Corner cases pass cases: {}/{}".format(len(corners_pass), len(corners_pass)))

for case in corners_fail:
    command = prefix + case
    result = subprocess.run(command.split(), stdout=subprocess.PIPE)

    accepted, retval = result.stdout.decode("utf-8").split()

    if accepted == "(UNACCEPTED)":
        if retval != "false":
            print("Test case failed: ", command, " -> ", retval)
            num += 1
print("Corner cases fail cases: {}/{}".format(len(corners_fail), len(corners_fail)))
