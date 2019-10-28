$fn=100;
breite=80;          // Aussenabmesungen der Platte
hoehe=5;            //
durchmesser=23.5;   // Oeffnung fuer den PIR-Sensor
materialstaerke=3;  
laenge_PIR=32.2;    // Laenge der PIR-Platine; Quelle: https://components101.com/hc-sr501-pir-sensor

difference() {  // Koerper
  // Aussen
  translate([-breite/2,-breite/2,0]) roundedRectangle(
      breite,
      breite,
      hoehe,
      2,
      center=false
    );
  // Aussparung
  translate([-(breite-materialstaerke)/2,-(breite-materialstaerke)/2,(-materialstaerke)+1.5]) roundedRectangle(
      breite-materialstaerke,
      breite-materialstaerke,
      hoehe,
      2,
      center=false
    );
  cylinder(d=durchmesser,10);
}

// die 4 Federn, die den Schalter in der UP-Dose halten
translate([-10,20,0]) feder();                      // links
rotate([0,0,180]) translate([-10,20,0]) feder();    // rechts
rotate([0,0,90]) translate([-10,20,0]) feder();     // unten
rotate([0,0,-90]) translate([-10,20,0]) feder();    // oben

// die 2 Federn, die die PIR-Platine halten
translate([laenge_PIR/2,0,-1]) rotate([0,5,0]) cube([1,10,10], center=true);
translate([-laenge_PIR/2,0,-1]) rotate([0,-5,0]) cube([1,10,10], center=true);

// Testbloecke
//  PIR-Platine
//translate([0,0,2]) cube([32.2, 24.3, 1], center=true);
//
// Aussparung fuer UP-Dose
// translate([0,0,-35]) cylinder(d=50,35);

module roundedRectangle(width, depth, height, radius, center=false) {
    translate([radius,radius,0]) {
        minkowski() {
            cube([width-2*radius,depth-2*radius,height/2], center=center);
            cylinder(r=radius,h=height/2);
        }
    }
}

module feder() {
  //Feder
  translate([10,1,3.8])
    rotate([0,90,0]) 
      linear_extrude(height = 20, center = true, convexity = 10, twist = 0) polygon(points = [ 
        [0,4], 
        [5,4],
        [20,7], 
        [25,7],
        [30,6], 
        [30,5],
        [25,6],
        [20,6], 
        [5,0],
        [0,0]
        ]);
  // Fuß
  translate([10,3,1.5]) cylinder(d1=materialstaerke, d2=18, h=materialstaerke+1, center=true);
}

