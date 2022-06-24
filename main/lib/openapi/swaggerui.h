/*
 * swaggerui.h
 *
 *  Created on: Jun 29, 2021
 *      Author: Bas
 */

#ifndef MAIN_SWAGGERUI_H_
#define MAIN_SWAGGERUI_H_

const char *swaggerJSON = R"====(
{
  "swagger": "2.0",
  "info": {
    "title": "IoT application",
    "version": "1.0.0"
  },
  "paths": {
    "/temperature": {
      "get": {
        "responses": {
          "200": {
            "description": "A list of temperature measurements",
            "schema": {
              "$ref": "#/definitions/temps"
            }
          }
        }
      }
    }
  },
  "definitions": {
    "temps": {
      "type": "array",
      "items": {
        "$ref": "#/definitions/temp"
      }
    },
    "temp": {
      "type": "object",
      "properties": {
        "value": {
          "type": "string"
        },
        "timestamp": {
          "type": "integer"
        }
      }
    }
  }
}
)====";


std::string GetSwaggerUIHTML(std::string jsonUrl)
{
	return std::string (R"====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta http-equiv="x-ua-compatible" content="IE=edge">
  <title>Swagger UI</title>

  <link href='https://cdnjs.cloudflare.com/ajax/libs/meyer-reset/2.0/reset.min.css' media='screen' rel='stylesheet' type='text/css'/>
  <link href='https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/css/screen.css' media='screen' rel='stylesheet' type='text/css'/>

  <script> 
    if (typeof Object.assign != 'function') {
      (function () {
        Object.assign = function (target) {
          'use strict';
          if (target === undefined || target === null) {
            throw new TypeError('Cannot convert undefined or null to object');
          }
          var output = Object(target);
          for (var index = 1; index < arguments.length; index++) {
            var source = arguments[index];
            if (source !== undefined && source !== null) {
              for (var nextKey in source) {
                if (Object.prototype.hasOwnProperty.call(source, nextKey)) {
                  output[nextKey] = source[nextKey];
                }
              }
            }
          }
          return output;
        };
      })();
    }
  </script>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery/1.8.0/jquery-1.8.0.min.js' type='text/javascript'></script>
  <script> 
    (function(b){b.fn.slideto=function(a){a=b.extend({slide_duration:"slow",highlight_duration:3E3,highlight:true,highlight_color:"#FFFF99"},a);return this.each(function(){obj=b(this);b("body").animate({scrollTop:obj.offset().top},a.slide_duration,function(){a.highlight&&b.ui.version&&obj.effect("highlight",{color:a.highlight_color},a.highlight_duration)})})}})(jQuery);
  </script>

  <script> 
    jQuery.fn.wiggle=function(o){var d={speed:50,wiggles:3,travel:5,callback:null};var o=jQuery.extend(d,o);return this.each(function(){var cache=this;var wrap=jQuery(this).wrap('<div class="wiggle-wrap"></div>').css("position","relative");var calls=0;for(i=1;i<=o.wiggles;i++){jQuery(this).animate({left:"-="+o.travel},o.speed).animate({left:"+="+o.travel*2},o.speed*2).animate({left:"-="+o.travel},o.speed,function(){calls++;if(jQuery(cache).parent().hasClass('wiggle-wrap')){jQuery(cache).parent().replaceWith(cache);}
    if(calls==o.wiggles&&jQuery.isFunction(o.callback)){o.callback();}});}});};
  </script>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery.ba-bbq/1.2.1/jquery.ba-bbq.min.js' type='text/javascript'></script>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/handlebars.js/4.0.5/handlebars.min.js' type='text/javascript'></script>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/lodash-compat/3.10.1/lodash.min.js' type='text/javascript'></script>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/backbone.js/1.1.2/backbone-min.js' type='text/javascript'></script>
  <script> 
    Backbone.View = (function(View) {
     return View.extend({
          constructor: function(options) {
              this.options = options || {};
              View.apply(this, arguments);
          }
      });
    })(Backbone.View);
  </script>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/swagger-ui.min.js' type='text/javascript'></script>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.10.0/highlight.min.js' type='text/javascript'></script>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.10.0/languages/json.min.js' type='text/javascript'></script>
 
  <script src='https://cdnjs.cloudflare.com/ajax/libs/json-editor/0.7.28/jsoneditor.min.js' type='text/javascript'></script>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/marked/0.3.6/marked.min.js' type='text/javascript'></script>


  <script type="text/javascript">
    $(function () {
      url = ")====") + jsonUrl + std::string(R"====(";
      hljs.configure({
        highlightSizeThreshold: 5000
      });
    
      window.swaggerUi = new SwaggerUi({
        url: url,
        dom_id: "swagger-ui-container",
        supportedSubmitMethods: ['get', 'post', 'put', 'delete', 'patch'],
		validatorUrl: null,
        onComplete: function(swaggerApi, swaggerUi){
  
        },
        onFailure: function(data) {
          log("Unable to Load SwaggerUI");
        },
        docExpansion: "none",
        jsonEditor: false,
        defaultModelRendering: 'schema',
        showRequestHeaders: false,
        showOperationIds: false
      });
      window.swaggerUi.load();
      function log() {
        if ('console' in window) {
          console.log.apply(console, arguments);
        }
      }
  });
  </script>
</head>

<body class="swagger-section">
<div id='header'>
  <div class="swagger-ui-wrap">
    <a id="logo" href="http://swagger.io"><img class="logo__img" alt="swagger" height="30" width="30" src="https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/images/logo_small.png" /><span class="logo__title">swagger</span></a>
    <form id='api_selector'>
     
    
    </form>
  </div>
</div>

<div id="message-bar" class="swagger-ui-wrap" data-sw-translate>&nbsp;</div>
<div id="swagger-ui-container" class="swagger-ui-wrap"></div>
</body>
</html>
)====");
}


#endif /* MAIN_SWAGGERUI_H_ */
