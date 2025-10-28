if (!!window.EventSource) {
    var source = new EventSource('/');
    source.onmessage = function(e) {
      var bumper = e.data[1]
      var cliff = e.data[3];
      var drop = e.data[5];


      // finish the code to handle the bumper status
      if (bumper == "0") {
        document.getElementById("bumperStatus").value = "OFF";
      }
      if (bumper == "1") {
        document.getElementById("bumperStatus").value = "ON";
      }
         
        
        // finish the code to handle the wheel drop status 
      if (drop == "0") {
        document.getElementById("wheelDropStatus").value = "OFF";
      }
      if (drop == "1") {
        document.getElementById("wheelDropStatus").value = "ON";
      }

      // finish the code to handle cliff status 
      if (cliff == "0") {
        document.getElementById("cliffStatus").value = "OFF";
      }
      if (cliff == "1") {
        document.getElementById("cliffStatus").value = "ON";
      }
        


    }
  }