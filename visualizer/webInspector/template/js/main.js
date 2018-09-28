window.define = ace.define;
window.require = ace.require;
window.onload = initControl;

var Range = require('ace/range').Range;

var _EDITOR;												//global variables
var _MARKER_SET = new Array;
var _LAST_ACTIVE_FAULT;
var _LAST_ACTIVE_NODE;
var _FAULT_SEVERITY_ONLY_SHOW_ERROR = false;
var _FAULT_CONFIDENCE_ONLY_SHOW_MUST = false;
var _INIT_CODE = `No source codes to display.
Choose a fault or select a source file to show codes.`;
var _CURRENT_FILE;

function initControl(){
	initEditor();
	initFileTree();
	initFaultsSet();
	initFaultsFilter();
	initPath();
}
			
function initEditor(){
	_EDITOR = ace.edit("editor");
	_EDITOR.renderer.setHScrollBarAlwaysVisible(false);
	_EDITOR.setTheme("ace/theme/eclipse");
	_EDITOR.session.setMode('ace/mode/java');
	_EDITOR.renderer.setShowGutter(true);
	_EDITOR.setShowPrintMargin(true);
	_EDITOR.setDisplayIndentGuides(true);
	_EDITOR.setHighlightSelectedWord(true);
	_EDITOR.setPrintMarginColumn(80);
	_EDITOR.setAnimatedScroll(true);
	_EDITOR.setHighlightActiveLine(false);
	
	_EDITOR.setReadOnly(true);
	_EDITOR.session.setValue(_INIT_CODE);
	_EDITOR.session.setTabSize(4);
	_EDITOR.session.setUseWrapMode(false);
}

function initFileTree(){
	$('#file_tree').fileTree({ root: " ", script: 'http://localhost:8080', expandSpeed: 1, collapseSpeed: 1 }, function(file) { 
					loadFile(file);
				});
}

function initFaultsSet(){
	var tempSet = _FAULTS_SET.concat();
	var resultSet = new Array();
	for(var faultIndex in tempSet){
		if(_FAULT_CONFIDENCE_ONLY_SHOW_MUST && tempSet[faultIndex].confidence != "MUST"){
			continue;
		}
		if(_FAULT_SEVERITY_ONLY_SHOW_ERROR && tempSet[faultIndex].severity != "ERROR"){
			continue;
		}
		resultSet.push(tempSet[faultIndex]);
	}
	$("#tablebody").find("*").remove();
	for(var faultIndex in resultSet){
		$("#tablebody").append('<tr class = "fault"><td class = "fault_id">' + resultSet[faultIndex].id + 
							   "</td>" + "<td>" + resultSet[faultIndex].severity + 
							   "</td>" +"<td>" + resultSet[faultIndex].confidence + 
							   "</td>" + "<td title='" + resultSet[faultIndex].weakness + "'>" + resultSet[faultIndex].weakness + "</td></tr>");
	}
	$(".fault").click(loadFaultPath);
}

function initFaultsFilter(){
	$('#fault_severity').checkbox({onChange:function(){
		_FAULT_SEVERITY_ONLY_SHOW_ERROR = !_FAULT_SEVERITY_ONLY_SHOW_ERROR;
		initFaultsSet();
		initPath();
	}});
	$('#fault_confidence').checkbox({onChange:function(){
		_FAULT_CONFIDENCE_ONLY_SHOW_MUST = !_FAULT_CONFIDENCE_ONLY_SHOW_MUST;
		initFaultsSet();
		initPath();
	}});
}

function initPath(){
	$("#path_list").find('*').remove();
	$("#path_list").append("<i><h4>Currently no fault is selected!</h4></i>");
}

function loadFile(filepath){
    removeOldMarkers();
    _EDITOR.session.clearAnnotations();
    if(filepath == _CURRENT_FILE){
        return;
    }
    $.get('http://localhost:8080', { file: filepath }, function(ret) {
			_CURRENT_FILE = filepath;
            _EDITOR.session.setValue(unescape(ret));
	});
}

function loadFaultPath(){
	$(_LAST_ACTIVE_FAULT).removeClass("active");
	_LAST_ACTIVE_FAULT = this;
	$(this).addClass("active");
	$("#path_list").find('*').remove();
	removeOldMarkers();
	var fault_id = $(this).find("td.fault_id").html();
	var path = faultID_Path_Dic["" + fault_id];
	for(var nodeIndex in path){
		$("#path_list").append("<a class='item pathNode' startline = '" + path[nodeIndex].startline + "'" + "endline = '" + path[nodeIndex].endline + "'" 
								+ "filePath = '" + path[nodeIndex].filePath + "'" 
								+ "><div class='ui middle aligmed content'><div class='header'>line:" + path[nodeIndex].startline
								+ '</div></div></a>');
	}
	$(".pathNode").click(addMarker);
	loadFile(path[0].filePath);
}

function removeOldMarkers(){
	for(var i in _MARKER_SET){
		_EDITOR.session.removeMarker(_MARKER_SET[i]);
	}
	_MARKER_SET = new Array;
}

function addMarker(){
	$(_LAST_ACTIVE_NODE).find("div.header").removeClass("ui brown inverted _active");
	$(_LAST_ACTIVE_NODE).find("p").removeClass("_active");
	$(_LAST_ACTIVE_NODE).removeClass("_active");
	_LAST_ACTIVE_NODE = this;
	$(this).find("p").addClass("_active");
	$(this).find("div.header").addClass("ui brown inverted _active");
	$(this).addClass("_active");
	
	loadFile($(this).attr('filePath'));
	var start = Number($(this).attr('startline'));
	var end = Number($(this).attr('endline'));
	var markerProduced = _EDITOR.session.addMarker(new Range(start - 1, 0, end, 0), 'Code-Marker', 'line', false);
	_EDITOR.session.setAnnotations([{
		row : start - 1,
		column: -1,
		text: "say\nsomething\nhere\nin fact, you can write sth really long here, it's good\nand easy to use",
		type: "info"
	}]);
	_EDITOR.scrollToLine(start - 1, true, true);
	_MARKER_SET.push(markerProduced);
}

