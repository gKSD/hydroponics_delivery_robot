$(function() {
	$('.editingredients').bind('click', function() {
		onclick_edit_ingredients();
	});

	$('#edit-ingredient-new-button').click(function() {
		edit_ingredient_new_handler();
	});

	$('#edit-ingredient-save-button').click(function() {
		edit_ingredient_save_handler();
	});

	$('#edit-ingredient-delete-button').click(function() {
		edit_ingredient_delete_handler();
	});

	$('#edit-ingredient-name').change(function() {
		update_edit_ingredient_description($('#edit-ingredient-name').val()); //FIXME is the same func as for gardenbed form
	});

});

function onclick_edit_ingredients() {
	var json_data = {}; 
	json_data['type'] = 'ingredients'; //FIXME create just ingredients-request JSON
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/get_params/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: first_fill_edit_ingredients_form,
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}

function first_fill_edit_ingredients_form(json) {
	var edit_ingredient_name = $("#edit-ingredient-name");
	edit_ingredient_name.find('option').remove()
	ingredients = json.ingredients;
	for (var property in ingredients) {
    	if (ingredients.hasOwnProperty(property)) {
        	edit_ingredient_name.append('<option value="' + property + '">' + ingredients[property] + '</option>');
    	}
	}
	update_edit_ingredient_description(edit_ingredient_name.val());
}

function edit_ingredient_new_handler(json) {
	var json_data = {};
	json_data['type'] = 'new';
	ingredient_name = prompt("Пожалуйста, введите название ингредиента", "Название ингредиента");
	if (ingredient_name == '' || ingredient_name == null) {
		return
	}
	ingredient_description = prompt("Пожалуйста, введите описание ингредиента", "Описание ингредиента");		
	if (ingredient_description == '' || ingredient_description == null) {
		return
	}
	json_data['ingredient_name'] = ingredient_name;
	json_data['ingredient_description'] = ingredient_description;
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/set_ingredients/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: function(json) {
	    	$("#edit-ingredient-name").append('<option value="' + json['id'] + '">' + ingredient_name + '</option>');
	    	alert(JSON.stringify(json));
	    },

	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}

function edit_ingredient_save_handler(json) {
	var json_data = {};
	json_data['type'] = 'save';
	json_data['ingredient_id'] = $('#edit-ingredient-name').val();
	json_data['ingredient_description'] = $('#edit-ingredient-description').val();
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/set_ingredients/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: function(json) {alert(JSON.stringify(json))},
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}

function update_edit_ingredient_description(ingredient_id) {
	var json_data = {};
	json_data['type'] = 'ingredient_description';
	json_data['ingredient_id'] = ingredient_id;
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/get_params/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: function(json) {$("#edit-ingredient-description").text(json.ingredient_description)},
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}

function edit_ingredient_delete_handler(json) {
	var json_data = {};
	deleted_ingredient_id = $('#edit-ingredient-name').val();
	json_data['type'] = 'delete';
	json_data['ingredient_id'] = deleted_ingredient_id;
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/set_ingredients/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: function(json) {
	    	$('#edit-ingredient-name option[value='+deleted_ingredient_id+']').remove();
	    	update_edit_ingredient_description($('#edit-ingredient-name').val());
	    	alert(JSON.stringify(json));
	    },
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}
