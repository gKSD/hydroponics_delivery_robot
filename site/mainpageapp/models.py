from django.db import models
from django.forms import ModelForm

class Plant(models.Model):
	class Meta():
		db_table = 'plant'
	plant_name = models.CharField(max_length=256, unique=True)
	plant_description = models.TextField()
	plant_image_path = models.CharField(max_length=512, unique=True)

class Ingredient(models.Model):
	class Meta():
		db_table = 'ingredient'
	ingredient_name = models.CharField(max_length=256, unique=True)
	ingredient_description = models.TextField()

class Proportion(models.Model):
	class Meta():
		db_table = 'proportion'
	proportion_ingredient = models.ForeignKey(Ingredient, blank=True, null=True)
	proportion_ingredient_amount = models.IntegerField(default=0)

class Mix(models.Model):
	class Meta():
		db_table = 'mix'
	mix_name = models.CharField(max_length=256, unique=True)
	mix_description = models.CharField(max_length=256)
	mix_proportions = models.ManyToManyField(Proportion, blank=True)

class GardenBed(models.Model):
	class Meta():
		db_table = 'gardenbed'
	gardenbed_posx = models.IntegerField(default=0)
	gardenbed_posy = models.IntegerField(default=0)
	gardenbed_name = models.CharField(max_length=256, unique=True)
	gardenbed_time = models.CharField(max_length=512, blank=True, null=True)
	gardenbed_plant = models.ForeignKey(Plant, blank=True, null=True)
	gardenbed_mix = models.ForeignKey(Mix, blank=True, null=True)

	def get_gardenbed_posx(self):
		return self.gardenbed_posx
		
	def get_gardenbed_posy(self):
		return self.gardenbed_posy

class Task(models.Model):
	class Meta():
		db_table = 'task'
	task_gardenbed = models.ForeignKey(GardenBed)
	task_time = models.IntegerField(default=0)
	task_json = models.CharField(max_length=1024)
	task_sent_to_base = models.IntegerField(default=0)	

class Base(models.Model):
	class Meta():
		db_table = 'base'
	base_name = models.CharField(max_length=256)
	base_ip = models.CharField(max_length=15)

class Tank(models.Model):
	class Meta():
		db_table = 'tank'
	tank_id = models.IntegerField(default=0)
	tank_base = models.ForeignKey(Base)
	tank_current_volume = models.IntegerField(default=0)
	tank_max_volume = models.IntegerField(default=0)
	tank_ingredient = models.OneToOneField(Ingredient, blank=True, null=True)

class Robot(models.Model):
	class Meta():
		db_table = 'robot'
	robot_name = models.CharField(max_length=256)
	robot_tank_volume = models.IntegerField(default=0)
	robot_ip = models.CharField(max_length=15)	