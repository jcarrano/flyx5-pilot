#! /bin/env python2
"""Convert midi files to C-arrays for use with the FLYX5 buzzer module"""

from __future__ import unicode_literals, division, print_function
import midi
from collections import namedtuple

__author__ = "Juan I Carrano"

_Note = namedtuple('_Note', 'pitch velocity duration')
MidiNote = namedtuple('_Note', 'onset pitch velocity duration')

VELOCITY_SILENCE = 0
DURATION_REPEAT = 0xFF
DURATION_STOP = 0xFF-1

class Note(_Note):
	def __str__(self):
		return "{%d, %d, %d}"%self

	@classmethod
	def silence(cls, duration):
		return cls(0, VELOCITY_SILENCE, duration)

	@classmethod
	def stop(cls):
		return cls(0, 0, DURATION_STOP)

	@classmethod
	def repeat(cls):
		return cls(0, 0, DURATION_REPEAT)

def track_to_list(track):
	activation = {}
	ticks = 0
	score = []

	for n in track:
		ticks += n.tick
		if n.name == 'Note Off' or (n.name == 'Note On' and n.get_velocity() == 0):
			pitch = n.get_pitch()
			try:
				prev_tick, previous_on = activation[pitch]
				del activation[pitch]
			except KeyError:
				print("Note Off at pitch %d does not have note on"%pitch)
			else:
				note_duration = ticks - prev_tick
				score.append(MidiNote(prev_tick, pitch, n.get_velocity(), note_duration))
		elif n.name == 'Note On':
			pitch = n.get_pitch()
			activation[pitch] = (ticks, n)

	return score

def list_to_notes(track, repeat = False):
	notelist = []
	#min_duration = min(n.duration for n in l)
	_duration = max(n.duration for n in l)

	for note in track:
		pass

def _print_usage():
	print("%s <midi_file> [channel] [repeat|once]"%sys.argv[0], file = sys.stderr)

def main():
	import sys

	try:
		filename = sys.argv[1]
	except IndexError:
		print("Must specify a filename", file = sys.stderr)

	try:
		chan_n = int(sys.argv[2])
	except IndexError:
		chan_n = 0

	try:
		repeat = sys.argv[3] == 'repeat'
	except IndexError:
		repeat = False

	pattern = midi.read_midifile(filename)
	print("Tracks:", file = sys.stderr)
	for n, t in enumerate(pattern):
		try:
			track_name = next(x.text for x in t if x.name == 'Track Name')
		except StopIteration:
			track_name = '???'
		print(n, track_name, file = sys.stderr)

	tempo = next(x.text for x in t if x.name == 'Track Name')

	chan = pattern[chan_n]

	l = track_to_list(chan)
	print("Durations:", " ".join(str(k) for k in set(n.duration for n in l)), file = sys.stderr)

if __name__ == '__main__':
	main()
