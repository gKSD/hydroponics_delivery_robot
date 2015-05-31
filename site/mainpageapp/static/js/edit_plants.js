$(function() {
	$('.editplants').bind('click', function() {
		onclick_edit_plants();
	});

	/*$('#edit-plant-new-button').click(function() {
		edit_plant_new_handler();
	});*/

	$('#edit-plant-save-button').click(function() {
		edit_plant_save_handler();
	});

	$('#edit-plant-delete-button').click(function() {
		edit_plant_delete_handler();
	});

	$('#edit-plant-name').change(function() {
		update_edit_plant_description($('#edit-plant-name').val()); //FIXME is the same func as for gardenbed form
	});

});

function onclick_edit_plants() {
	var json_data = {}; 
	json_data['type'] = 'plants_mixs'; //FIXME create just plants-request JSON
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/get_params/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: first_fill_edit_plants_form,
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}

function first_fill_edit_plants_form(json) {
	var edit_plant_name = $("#edit-plant-name");
	edit_plant_name.find('option').remove()
	plants = json.plants;
	for (var property in plants) {
    	if (plants.hasOwnProperty(property)) {
        	edit_plant_name.append('<option value="' + property + '">' + plants[property] + '</option>');
    	}
	}
	update_edit_plant_description(edit_plant_name.val());
}

function edit_plant_new_handler(json) {
	/*var json_data = {};
	json_data['type'] = 'new';
	plant_name = prompt("Пожалуйста, введите название растения", "Название растения");
	if (plant_name == '' || plant_name == null) {
		return
	}
	plant_description = prompt("Пожалуйста, введите описание растения", "Описание растения");		
	if (plant_description == '' || plant_description == null) {
		return
	}
	json_data['plant_name'] = plant_name;
	json_data['plant_description'] = plant_description;
	json_data['plant_image_path'] = plant_name + 'image.png'; //FIXME temp
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/set_plants/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: function(json) {
	    	$("#edit-plant-name").append('<option value="' + json['id'] + '">' + plant_name + '</option>');
	    	alert(JSON.stringify(json));
	    },

	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});*/
}

function edit_plant_save_handler(json) {
	var json_data = {};
	json_data['type'] = 'save';
	json_data['plant_id'] = $('#edit-plant-name').val();
	json_data['plant_description'] = $('#edit-plant-description').val();
	json_data['plant_image_path'] = $('#edit-plant-name').val() + 'image.png'; //FIXME temp
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/set_plants/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: function(json) {alert(JSON.stringify(json))},
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}

function update_edit_plant_description(plant_id) {
	var json_data = {};
	json_data['type'] = 'plant_description';
	json_data['plant_id'] = plant_id;
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/get_params/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: function(json) {$("#edit-plant-description").text(json.plant_description)},
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}

function edit_plant_delete_handler(json) {
	var json_data = {};
	deleted_plant_id = $('#edit-plant-name').val();
	json_data['type'] = 'delete';
	json_data['plant_id'] = deleted_plant_id;
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/set_plants/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: function(json) {
	    	$('#edit-plant-name option[value='+deleted_plant_id+']').remove();
	    	update_edit_plant_description($('#edit-plant-name').val());
	    	alert(JSON.stringify(json));
	    },
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}