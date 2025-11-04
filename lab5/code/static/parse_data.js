if (!!window.EventSource) {
    var source = new EventSource('/');
    source.onmessage = function(e) {
      // var bumper = e.data[1]
      // var cliff = e.data[3];
      // var drop = e.data[5];
    var bumper = parseInt(e.data[1]);
    var cliff  = parseInt(e.data[3]);
    var drop   = parseInt(e.data[5]);


      // finish the code to handle the bumper status
      if (bumper == 0) {
        document.getElementById("bumperStatus").value = "No contact";
      } else {
        let status = [];
        //Testing each bit using AND operation
        if (bumper & 0x01) status.push("Right");
        if (bumper & 0x02) status.push("Center");
        if (bumper & 0x04) status.push("Left");
        document.getElementById("bumperStatus").value = status.join(" + ");
      }
         
        
      // finish the code to handle the wheel drop status 
      // 0x01 for right wheel, 0x02 for left wheel
      if (drop == 0) {
        document.getElementById("wheelDropStatus").value = "OK";
      } else {
        let dropStatus = [];
        if (drop & 0x01) dropStatus.push("Right Dropped");
        if (drop & 0x02) dropStatus.push("Left Dropped");
        document.getElementById("wheelDropStatus").value = dropStatus.join(" + ");
      }

      // finish the code to handle cliff status 
      // 0x01 for right cliff, 0x02 for center cliff, 0x04 for left cliff
      if (cliff == 0) {
        document.getElementById("cliffStatus").value = "Clear";
      } else {
        let cliffStatus = [];
        if (cliff & 0x01) cliffStatus.push("Right");
        if (cliff & 0x02) cliffStatus.push("Center");
        if (cliff & 0x04) cliffStatus.push("Left");
        document.getElementById("cliffStatus").value = cliffStatus.join(" + ");
      }
          


    }
  }