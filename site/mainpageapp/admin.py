from django.contrib import admin
from mainpageapp.models import Plant, GardenBed, Mix, Ingredient, Proportion, Tank, Task, Base, Robot

# Register your models here.
admin.site.register(Plant)
admin.site.register(Ingredient)
admin.site.register(Proportion)
admin.site.register(GardenBed)
admin.site.register(Mix)
admin.site.register(Tank)
admin.site.register(Task)
admin.site.register(Base)
admin.site.register(Robot)