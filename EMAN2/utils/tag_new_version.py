#!/usr/bin/env python

import argparse
import os

import requests
from enum import Enum
import subprocess as sp

class Version(Enum):
	MAJOR = 0
	MINOR = 1
	PATCH = 2


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--bump', choices=['major', 'minor', 'patch'], default='patch')

	options = parser.parse_args()
	bump_bit = options.bump.upper()
	print(f"Bumping '{bump_bit}' version...")

	tags = [t['name'] for t in requests.get('https://api.github.com/repos/cryoem/eman2/tags').json()]
	print(f"Received GitHub tags:\n{tags}")

	tags = sorted([t for t in tags if t.startswith('v')], reverse=True)
	print(f"Version tags (sorted: latest to oldest):\n{tags}")

	tag = tags[0]
	version_cur = tag[1:]

	print(f"Latest tag:\n{tag}")
	print(f"Current version:\n{version_cur}")

	version = [int(v) for v in version_cur.split('.')]

	while len(version) < 3:
		version.append(0)

	print(f"Version bits:\n{version}")

	i_bump = Version[bump_bit].value

	# Bump requested version bit
	version[i_bump] += 1

	# Reset lower version bits
	for i in range(i_bump + 1, len(version)):
		version[i] = 0

	print(f"Bumped version bits:\n{version}")

	version = '.'.join([str(i) for i in version])
	print(f"Bumped version:\n{version}")

	# Run subcommands
	print("Running subcommands...")

	GIT_BRANCH_SHORT = os.getenv('GIT_BRANCH_SHORT')

	for cmd in (
			f'git branch -D {GIT_BRANCH_SHORT}',
			f'git checkout -b {GIT_BRANCH_SHORT}',
			f'python utils/bump_version.py {version_cur} {version}',
			f'git commit -a -m v{version}',
			f'git push origin {GIT_BRANCH_SHORT}',
			f'git tag -f v{version}',
			f'git push origin v{version}',
			):
		print(f"> {cmd}")
		cmd = cmd.split()
		sp.run(cmd)


if __name__ == "__main__":
	main()
