$(function() {
	$('.delete-button').bind('click', function() {
		onclick_delete_button($(this).attr('mix-id'));
	});

	$('#add-mix-button').bind('click', function() {
		onclick_add_button();
	});
});

function onclick_delete_button(mix_id){
	var json_data = {};
	alert(mix_id);
	json_data['type'] = 'delete';
	json_data['mix_id'] = mix_id;
	json_str = JSON.stringify(json_data);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/set_mixs/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: function(json) {	    
	    	alert(JSON.stringify(json));
	    },
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}

function onclick_add_button() {
	var json_data = {};
	var ingredients = {};
	$('.add_ingredient_amount').each(function() {
		if ($(this).val() > 0) {
			ingredients[$(this).attr('ingredient-id')] = $(this).val();
		}
	});
	json_data['type'] = 'new';
	json_data['ingredients'] = ingredients;
	json_data['mix_name'] = $('#add_mix_name').val();
	json_data['mix_description'] = $('#add_mix_description').val();
	json_str = JSON.stringify(json_data);
	alert(json_str);
	$.ajax({    
	    type: 'POST',                                                                                                                                                                                            
	  	url: '/set_mixs/',
	  	data: json_str,                                                                                                                                              
	    dataType: 'json', 
	    success: function(json) {	    
	    	alert(JSON.stringify(json));
	    },
	    error: function(xhr, text_status, exception ) {alert(text_status + exception);}                                                                                                                                                                                           
   	});
}

