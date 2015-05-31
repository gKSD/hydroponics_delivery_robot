from django.conf.urls import include, url

urlpatterns = [
    # Examples:
    # url(r'^$', 'hydroplants.views.home', name='home'),
    # url(r'^blog/', include('blog.urls')),

    #url(r'^$', 'mainpageapp.views.authenticate'),
    url(r'^$', 'mainpageapp.views.enter'),
    url(r'auth/', 'mainpageapp.views.authenticate'),
    url(r'login/', 'mainpageapp.views.login'),
    url(r'logout/', 'mainpageapp.views.logout'),
    url(r'home/', 'mainpageapp.views.home'),
    url(r'get_params/', 'mainpageapp.views.get_params'),
    url(r'set_params/', 'mainpageapp.views.set_params'),
    url(r'set_plants/', 'mainpageapp.views.set_plants'),
    url(r'set_ingredients/', 'mainpageapp.views.set_ingredients'),
    url(r'add_plant/', 'mainpageapp.views.add_plant'),
    url(r'set_mixs/', 'mainpageapp.views.set_mixs'),
    url(r'mixs/', 'mainpageapp.views.mixs'),
]