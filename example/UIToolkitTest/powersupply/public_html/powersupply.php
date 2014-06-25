<!DOCTYPE html>
<html>
  <head>
    <link href="powersupply.css" rel="stylesheet" type="text/css">
      <script type="text/Javascript" src="webChaos/jquery.js"></script>
       <script type="text/Javascript" src="webChaos/flot/jquery.flot.js"></script>

        <script type="text/javascript" src="Chaos.js"></script>        
        <script>
<?php
$con=mysqli_connect("mdsserver","root","mafalda0","chaosms");
// Check connection
if (mysqli_connect_errno()) {
  echo "Failed to connect to MySQL: " . mysqli_connect_error();
}
 $result = mysqli_query($con,"select distinct name,device_identification from dataset_attribute d inner join devices dd where d.id_device=dd.id_device order by timestamp desc;");
$cu_string="";
$cus=array();
 while($row = mysqli_fetch_array($result)) {
  if($row['name']== "current_sp"){
      //$cu_string=$cu_string."\"".$row['device_identification']."\",";
      array_push($cus,"\"".$row['device_identification']."\"");
  }
  
}

mysqli_close($con);
  //  $cus= array("\"simpower\"","\"SQUATB003\"","\"SQUATB004\"","\"DHSTB002\"");
    $cu_string=implode(",",$cus);
    echo "initializePowerSupply([$cu_string]);";
          
?>            
        </script>
        <body onload="setInterval(powerSupplyUpdateArrayInterface, 1500);">
        <table>
            <tr>
                <td colspan="5"><b>State</b></td>
                <td colspan="2"><b>Current</b></td>
                <td><b>Current SP</b></td>
                <td><b>Current Readout</b></td>
                <td><b>Seconds</b></td>

                <td colspan="2"><b>Alarms</b></td>
             </tr>

<?php
for($i=0;$i<count($cus);$i++){
echo "<tr>"; 
echo "<td><b id=\"cuname_$i\" class=\"PowersupplyIndicator\">$cus[$i]</b></td>";
echo "<td><input type=\"button\" name=\"on\" id=\"onstby_$i\" value=\"On\" onclick=\"cus[$i].powerSupplyToggle(this.value);\" /></td>";
 echo "<td><input type=\"button\" name=\"pos\" id=\"pos_$i\" value=\"Pos\" onclick=\"cus[$i].setPolarity(1);\" /></td>";
echo "<td><input type=\"button\" name=\"neg\" id=\"neg_$i\" value=\"Neg\" onclick=\"cus[$i].setPolarity(-1);\" /></td>";
echo "<td><input type=\"button\" name=\"open\" id=\"open_$i\" value=\"Open\" onclick=\"cus[$i].setPolarity(0);\" /></td>";
echo "  <td><input type=\"text\" id=\"inputcurrent_$i\" name=\"currentSet\" class=\"controlInput\" value=\"0\" /></td> ";
echo "  <td><input type=\"button\" id=\"setcurrent_$i\" value=\"set\" onclick=\"cus[$i].setCurrent(inputcurrent_$i.value);\" />";
echo "<td><b id=\"spcurrent_$i\" class=\"PowersupplyIndicator\">NA</b></td>";
echo "  <td><b id=\"readoutcurrent_$i\" class=\"PowersupplyIndicator\">NA</b></td>";
echo "  <td><b id=\"timestamp_$i\" class=\"PowersupplyIndicator\">NA</b></td>";
 echo "  <td><input type=\"button\" name=\"rstalarms\" id=\"rstalarms_$i\" value=\"rstalarms\" onclick=\"cus[$i].powerSupplyClrAlarms();\" /></td>";
 echo "  <td><b id=\"readoutalarms_$i\" class=\"PowersupplyIndicator\">NA</b></td>";
 echo " <td><div id=\"powersupply-graph_$i\" class=\"powersupply-graph-class\"></div></td>";	
echo "</tr>";
}
?>

  </tr>
                </table>
    </body>
</html>
