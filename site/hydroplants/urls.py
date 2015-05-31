from django.conf.urls import include, url
from django.contrib import admin
from django.contrib.staticfiles.urls import staticfiles_urlpatterns

urlpatterns = [
    url(r'^admin/', include(admin.site.urls)),
    url(r'^', include('mainpageapp.urls')),
    url(r'^base/', include('baseapp.urls')),
    url(r'^robot/', include('robotapp.urls')),
]

urlpatterns += staticfiles_urlpatterns()
