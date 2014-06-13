/**
* @file
* Js script to produce the graphics in statistics page.
*/

(function () {
  "use strict";
  
jQuery(document).ready(function($) {
  var usersobject = Drupal.settings.ces_statistics.staticsusers;
  var activityobject = Drupal.settings.ces_statistics.staticsactivity;
  var transobject = Drupal.settings.ces_statistics.staticstrans;
  var usersarray = new Array();
  var activityarray = new Array();
  var activityarray2 = new Array();
  var amountarray = new Array();
  var numberarray = new Array();

  $.each(usersobject, function (index, order) {
    usersarray.push([order.usersdate,order.usersnumber]);
  });

  $.each(activityobject, function (index, order) {
    activityarray.push([order.activitylevel,parseInt(order.activitypercent)]);
    activityarray2.push([parseInt(order.activitypercent) + '%(' + order.activitynumber + ')']);
  });

  $.each(transobject, function (index, order) {
    amountarray.push([order.transdate,parseFloat(order.transamount).toFixed(0),parseFloat(order.transamount).toFixed(2)]);
    numberarray.push([order.transdate,order.transnumber]);
  });

  // Number of accounts chart
  $.jqplot('chartdiv1', [usersarray], {
    axes:{
      xaxis:{
        renderer:$.jqplot.DateAxisRenderer,
    tickRenderer: $.jqplot.CanvasAxisTickRenderer,
    tickOptions:{angle: 30,formatString:'%b-%y'},
    min:usersarray[0][0],
    tickInterval:'1 month',
      },
    yaxis:{
      autoscale:true,
    min: 0,
    },
    },
    series:[{
      xaxis:'xaxis',
    yaxis:'yaxis',
    color: "#66AA00",
    rendererOptions: {smooth: true},
    pointLabels:{show:true, stackedValue: false},
    }],
    legend:{
      show: false,
    }
  });

  // Last year accounts' activity chart
  $.jqplot ('chartdiv2', [activityarray], {
    seriesColors: ["#eeffaa", "#bbee55", "#aadd44", "#99cc33", "#88bb22"],
    seriesDefaults: {
      renderer: jQuery.jqplot.PieRenderer,
    rendererOptions: {showDataLabels: true, dataLabels: activityarray2, startAngle: 270}
    },
    legend: {show:true, location: 'e'},
  });

  // Number and amount of transactions chart
  $.jqplot('chartdiv3', [amountarray,numberarray], {
    axes:{
      xaxis:{
        renderer:$.jqplot.DateAxisRenderer,
    tickRenderer: $.jqplot.CanvasAxisTickRenderer,
    tickOptions:{angle: 30,formatString:'%b-%y'},
    min:amountarray[0][0],
    tickInterval:'1 month',
      },
    yaxis:{
      autoscale:true,
    min: 0,
    },
    },

    seriesDefaults: {
      pointLabels:{show:true, stackedValue: false},
    },
    series:[{
      renderer:$.jqplot.BarRenderer,
      rendererOptions: {shadowAlpha: 0, barWidth: 20, barPadding: 20, barDirection: 'vertical'},
      xaxis:'xaxis',
      yaxis:'yaxis',
      color: "#99CC33",
    },{
      xaxis:'xaxis',
      color: "#66AA00",
    }],
    legend:{
      show: false,
    }
  });
});

})();
