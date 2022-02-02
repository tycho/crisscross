from filecmp import cmp
from os import path, remove, replace
from shutil import which
from subprocess import run, PIPE
from sys import argv, exit

if len(argv) < 2:
	exit("Error: missing output file")
elif len(argv) < 3:
	exit("Error: missing prefix")
elif len(argv) < 4:
	exit("Error: missing short prefix")

output=argv[1]
prefix=argv[2]
smPrefix=argv[3]

scriptPath = path.dirname(__file__)
tag = ""
build = 0
git_tag = ""

if path.exists(scriptPath + "/../.git") and which("git"):
	result = run('git describe --tags --abbrev=0', cwd=scriptPath, shell=True, stdout=PIPE)
	tag = result.stdout.decode("utf-8").strip("\n\r\t ")

if not tag:
	try:
		with open(scriptPath + '/release_ver', 'r') as file:
			tag = file.read()
		tag = tag.strip("\n\r\t ")
	except EnvironmentError as e: # parent of IOError, OSError *and* WindowsError where available
		print("Can't open release_ver:{0}".format(e))
		raise
else:
	result = run('git rev-list --count {0}..HEAD'.format(tag), cwd=scriptPath, shell=True, stdout=PIPE)
	build = result.stdout.decode("utf-8").strip("\n\r\t ")

	result = run('git rev-parse --short HEAD'.format(tag), cwd=scriptPath, shell=True, stdout=PIPE)
	git_tag = "{0}-{1}-g{2}".format(tag, build, result.stdout.decode("utf-8").strip("\n\r\t "))

result = tag.split(".")

major = None
minor = None
revis = None

if result[0][0] == 'v':
	major = result[0][1:]
	minor = result[1]
	revis = result[2]
	build = result[3]
else:
	major = result[0]
	minor = result[1]
	revis = result[2]

data = None

with open(scriptPath + "/build_number.h.in", "rt") as file:
	data = file.read()

data = data.replace("@SM_PREFIX@", smPrefix)
data = data.replace("@PREFIX@", prefix)
data = data.replace("@VERSION_MAJOR@", major)
data = data.replace("@VERSION_MINOR@", minor)
data = data.replace("@VERSION_REVISION@", revis)
data = data.replace("@VERSION_BUILD@", build)
data = data.replace("@GIT_TAG@", git_tag)

with open(output + ".tmp", "wt") as file:
	file.write(data)

equal = False
try:
	equal = cmp(output + ".tmp", output, shallow=False)
except FileNotFoundError:
	pass

if not equal:
	replace(output + ".tmp", output)
else:
	remove(output + ".tmp")
