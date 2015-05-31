# -*- coding: utf-8 -*-
from django.shortcuts import render
from django.http.response import HttpResponse
from django.template.loader import get_template
from django.template import Context
from django.shortcuts import render_to_response, redirect
from django.http import HttpResponseRedirect
from django.core.context_processors import csrf
from django.contrib import auth

from mainpageapp.models import Plant, Mix, GardenBed, Ingredient, Proportion, Tank, Task, Base, Robot
from django.db.models import Max

import socket
from hydroplants.settings import ROBOT_SOCK
ROBOT_SOCK.connect(('localhost', 8091))
ROBOT_SOCK.send('OLOLOL \n');

import json
from django.http import HttpResponse

def send_to_robot(command):
	data = {}
	data['client'] = 'django'
	data['command'] = command
	myjson = json.dumps(data)
	print ROBOT_SOCK.send(myjson+'\r\n')

import time
import threading
import Queue
import atexit

def _worker():
    while True:
        func, args, kwargs = _queue.get()
        try:
            func(*args, **kwargs)
        except:
            pass # bork or ignore here; ignore for now
        finally:
            _queue.task_done() # so we can join at exit

def run_in_thread(func):
    def decorator(*args, **kwargs):
        _queue.put((func, args, kwargs))
    return decorator

_queue = Queue.Queue()
_thread = threading.Thread(target = _worker) # one is enough; it's postponed after all
_thread.daemon = True # so we can exit
_thread.start()

def _cleanup():
    #_queue.join() # so we don't exit too soon
    print "\nDebug: cleanup\n"

atexit.register(_cleanup)

@run_in_thread
def poll_robot():
	while True:
		data = ROBOT_SOCK.recv(1024)
		data.rstrip('\r\n')
		if data: 
			print "Recieved data: "
    		print "***"+data+"***"
    		print "\n"
    		if data.find('robot') >= 0:
    			print "found"
    		elif data.find('get_field_size') >= 0:
    			gardenbed_posx_max = GardenBed.objects.all().aggregate(Max('gardenbed_posx'))['gardenbed_posx__max']
    			gardenbed_posy_max = GardenBed.objects.all().aggregate(Max('gardenbed_posy'))['gardenbed_posy__max']
    			data = {}
    			data['gardenbed_posx_max'] = gardenbed_posx_max
    			data['gardenbed_posy_max'] = gardenbed_posy_max
    			data['client'] = 'django'
    			myjson = json.dumps(data)
    			print ROBOT_SOCK.send(myjson+'\r\n')
    			pass
    		elif data.find('get_base_pos') >= 0:
    			data = {}
    			data['base_pos'] = 'A1' #TODO: get it from db
    			data['client'] = 'django'
    			myjson = json.dumps(data)
    			print ROBOT_SOCK.send(myjson+'\r\n')
    			pass
    		elif data.find('get_tank_volume') >= 0:
    			volume = Robot.objects.get(robot_name = 'Робот 1').robot_tank_volume
    			data = {}
    			data['client'] = 'django'
    			data['robot_tank_volume'] = volume
    			myjson = json.dumps(data)
    			print ROBOT_SOCK.send(myjson+'\r\n')
    			pass
    		elif data.find ('get_base_ip') >= 0:
    			base_ip = Base.objects.get(base_name = 'База 1').base_ip
    			data = {}
    			data['client'] = 'django'
    			data['base_ip'] = base_ip
    			myjson = json.dumps(data)
    			print ROBOT_SOCK.send(myjson+'\r\n')
    			pass

		#time.sleep(5)


# Create your views here.
def load_robot(request):
	print "load robot again 111111111"
	print ROBOT_SOCK.send('it me django robot\n')
	poll_robot() #TODO: перенести вызов при старте
	return render_to_response('robot.html')

def btn_left_process(request):
	print " 22222"
	send_to_robot('turn_left')
	data = {}
	data['success'] = '1'
	return HttpResponse(json.dumps(data), content_type = "application/json")


def btn_right_process(request):
	print 'Is btn_right_process\n'
	send_to_robot('turn_right')
	data = {}
	data['client'] = 'django'
	data['success'] = '1'
	return HttpResponse(json.dumps(data), content_type = "application/json")

def btn_back_process(request):
	print 'Is btn_back_process\n'
	send_to_robot('run_back')
	data = {}
	data['client'] = 'django'
	data['success'] = '1'
	return HttpResponse(json.dumps(data), content_type = "application/json")

def btn_forward_process(request):
	print 'Is btn_forward_process\n'
	send_to_robot('run_forward')
	data = {}
	data['client'] = 'django'
	data['success'] = '1'
	return HttpResponse(json.dumps(data), content_type = "application/json")

def btn_taround_process(request):
	print 'Is btn_taround_process\n'
	send_to_robot('turn_around')
	data = {}
	data['client'] = 'django'
	data['success'] = '1'
	return HttpResponse(json.dumps(data), content_type = "application/json")

def btn_stop_process(request):
	print 'Is btn_stop_process\n'
	send_to_robot('stop_moving')
	data = {}
	data['client'] = 'django'
	data['success'] = '1'
	return HttpResponse(json.dumps(data), content_type = "application/json")

def btn_cam_right_process (request):
	print 'Is btn_cam_right_process \n'
	send_to_robot('turn_cam_right')
	data = {}
	data['client'] = 'django'
	data['success'] = '1'
	return HttpResponse(json.dumps(data), content_type = "application/json")

def btn_cam_left_process (request):
	print 'Is btn_cam_left_process \n'
	send_to_robot('turn_cam_left')
	data = {}
	data['client'] = 'django'
	data['success'] = '1'
	return HttpResponse(json.dumps(data), content_type = "application/json")

def btn_cam_stop_process (request):
	print 'Is btn_cam_stop_process \n'
	send_to_robot('stop_cam_moving')
	data = {}
	data['client'] = 'django'
	data['success'] = '1'
	return HttpResponse(json.dumps(data), content_type = "application/json")