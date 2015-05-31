from django.conf.urls import include, url

urlpatterns = [
    url(r'^$', 'robotapp.views.load_robot'),
    url(r'btn_left/', 'robotapp.views.btn_left_process'),
    url(r'btn_right/', 'robotapp.views.btn_right_process'),
    url(r'btn_back/', 'robotapp.views.btn_back_process'),
    url(r'btn_forward/', 'robotapp.views.btn_forward_process'),
    url(r'btn_taround/', 'robotapp.views.btn_taround_process'),
    url(r'btn_stop/', 'robotapp.views.btn_stop_process'),
    url(r'btn_cam_right/', 'robotapp.views.btn_cam_right_process'),
    url(r'btn_cam_left/', 'robotapp.views.btn_cam_left_process'),
    url(r'btn_cam_stop/', 'robotapp.views.btn_cam_stop_process'),
]