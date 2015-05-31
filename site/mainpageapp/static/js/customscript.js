current_gardenbed_id = 0;

$(function() {
	$('.openeditform').bind('click', function() {
		onclick_gardenbed($(this).attr('gardenbed-id'));
	});

	var json_data = {};
	json_data['type'] = 'plants_mixs';
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/get_params/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: first_fill_edit_form,
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});

   	$('#sel-plant-name').change(function() {
		$('#save-button').prop('disabled', false);
		update_plant_description($('#sel-plant-name').val());
	});

	$('#sel-plant-mix').change(function() {
		$('#save-button').prop('disabled', false);
	});

   	$('.chkbox').change(function() {
		$('#save-button').prop('disabled', false);
	});

	$('#save-button').click(function() {
		save_form_parameters();
	});

	$('#clear-all-button').click(function() {
		clear_all_form_parameters();
	});
});

function clear_all_form_parameters() {
	$('#sel-plant-name').val('');
	$('#sel-plant-mix').val('');
	$('#plant-description').text('');
	$('.chkbox').each(function() {
    	this.checked = false;            
    });
	$('#save-button').prop('disabled', false);
}

function save_form_parameters() {
	var json_data = {};
	json_data['type'] = 'set_gardenbed';
	json_data['gardenbed_id'] = current_gardenbed_id;
	json_data['gardenbed_plant_id'] = $('#sel-plant-name').val();
	json_data['gardenbed_mix_id'] = $('#sel-plant-mix').val();
	time = '';
	for (i = 0; i < 24; i++) {
		if ($('#t' + i).is(':checked')) {
			time += i;
			time += ',';
		}
	}
	json_data['gardenbed_time'] = time;
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/set_params/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: function(json) {alert(JSON.stringify(json))},
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}


function update_plant_description(plant_id) {
	var json_data = {};
	json_data['type'] = 'plant_description';
	json_data['plant_id'] = plant_id;
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/get_params/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: function(json) {$("#plant-description").text(json.plant_description)},
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}

function first_fill_edit_form(json) {
	var modal_sel_plant_name = $("#sel-plant-name");
	var modal_sel_plant_mix = $("#sel-plant-mix");
	plants = json.plants;
	mixs = json.mixs;
	for (var property in plants) {
    	if (plants.hasOwnProperty(property)) {
        	modal_sel_plant_name.append('<option value="' + property + '">' + plants[property] + '</option>');
    	}
	}
 	for (var property in mixs) {
    	if (mixs.hasOwnProperty(property)) {
        	modal_sel_plant_mix.append('<option value="' + property + '">' + mixs[property] + '</option>');
    	}
	}
}

/* Shows edit-form for current gardenbed */
function onclick_gardenbed(gb_id) {
	current_gardenbed_id = gb_id;
	var json_data = {};
	json_data['type'] = 'gardenbed';
	json_data['gardenbed_id'] = gb_id;
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/get_params/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: fill_edit_form,
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}

function fill_edit_form(json) {
	modal_title = $('#modal-title');
	modal_sel_plant_name = $('#sel-plant-name');
	modal_plant_description = $('#plant-description');
	modal_sel_plant_mix = $('#sel-plant-mix');
	gardenbed_name = 'Номер участка: ' + json.gardenbed_name;
	modal_title.text(gardenbed_name);

	gardenbed_plant_id = json.gardenbed_plant_id;
	if (gardenbed_plant_id) {
		modal_sel_plant_name.val(gardenbed_plant_id);
		gardenbed_plant_description = json.gardenbed_plant_description;
		if (modal_plant_description) {
			modal_plant_description.text(gardenbed_plant_description);
		} 
		else {
			modal_plant_description.text('');
		}
	} 
	else {
		modal_sel_plant_name.val('');
		modal_plant_description.text('');
	}
	gardenbed_mix_id = json.gardenbed_mix_id;
	if (gardenbed_mix_id) {
		modal_sel_plant_mix.val(gardenbed_mix_id);
	} 
	else {
		modal_sel_plant_mix.val('');
	}

	$('.chkbox').each(function() {
    	this.checked = false;            
    });
	gardenbed_time = json.gardenbed_time;
	if (gardenbed_time) {
		gardenbed_parsed_time = gardenbed_time.split(',');
		$.each(gardenbed_parsed_time, function(index, value) {
  			$('#t'+ value.replace(/\s+/g, '')).prop('checked', true);
		});
	}
	$('#save-button').prop('disabled', true);
}

function mouse_over() {
	//Show info
	//document.getElementById("button").src="../images/button2.png";
}
	
function mouse_out() {
	//Stop show info
	//document.getElementById("button").src="../images/button1.png";
}