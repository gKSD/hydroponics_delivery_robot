from django import forms
from django.core.exceptions import ValidationError
from mainpageapp.models import Ingredient, Proportion, Mix 

class MixForm (forms.ModelForm):
    class Meta:
        model = Mix
        fields = ['mix_name', 'mix_description', 'mix_proportions']
