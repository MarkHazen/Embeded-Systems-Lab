if (!!window.EventSource) {
    var source = new EventSource('/');
    source.onmessage = function(e) {
      var bumper = e.data[1]
      var cliff = e.data[3];
      var drop = e.data[5];


      // finish the code to handle the bumper status
        if (bumper=="0")
          {
            document.getElementById("but1").value = "OFF";
          }
        if (bumper=="1")
        {
          document.getElementById("but1").value = "Right";
        }
        
         
        
        // finish the code to handle the wheel drop status 
      if (drop=="0")
        {
          document.getElementById("the id of button where you need to display the sensor status").value = "OFF";

        }
      

      // finish the code to handle cliff status 
      if (cliff=="0")
        {
          document.getElementById("the id of button where you need to display the sensor status").value = "OFF";
        }
      


    }
  }