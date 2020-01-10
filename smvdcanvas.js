//credits for the base of the interaction-handling logic:
//https://dzone.com/articles/gentle-introduction-making
//rest by Nil M.

var planeOffset = 10;
var planeSize = 400;
var siteRadius = 4;
var siteWidth = 2;
var selectionColor = 'red';
var selectionWidth = 3;

var initMetric = 'euclidean';
var initNumSites = 10;
var initAppetite = 7;
var totalAppetite = 100; //don't touch this

var colors =
  ['#e6194b', '#3cb44b', '#ffe119', '#4363d8', '#f58231', '#911eb4', '#46f0f0', '#f032e6', '#bcf60c', '#fabebe', '#008080', '#e6beff', '#9a6324', '#fffac8', '#800000', '#aaffc3', '#808000', '#ffd8b1', '#000075', '#808080', '#E6FF80', '#00B3E6', '#E6B333', '#B34D4D',
  '#6680B3', '#991AFF']
//////////////////////////////
// utils
//////////////////////////////
function randBetween(i, j) { //both inclusive
  return Math.floor((Math.random() * (j-i+1)) + i);
}

//distance between (x1,y1) and (x2,y2)
function dist(x1, y1, x2, y2) {
  var a = x1 - x2;
  var b = y1 - y2;
  var c = Math.sqrt( a*a + b*b );
  return c;
}

/////////////////////////////
// Person class
/////////////////////////////
//unique id counter for sites
var siteId = 0;

// Constructor for points that will represent people.
class Site {
  constructor(x, y) {
    this.id = siteId;
    siteId++;
    this.setX(x);
    this.setY(y);
  }
  toString() {
    return '('.concat(this.x.toString()).concat(',').concat(this.y.toString()).concat(')');
  }
  setX(x) {
    var newX = x;
    if (newX < planeOffset)
      this.x = planeOffset;
    else if (newX >= planeSize+planeOffset)
      this.x = planeSize + planeOffset -1;
    else
      this.x = newX;
  }
  setY(y) {
    var newY = y;
    if (newY < planeOffset)
      this.y = planeOffset;
    else if (newY >= planeSize+planeOffset)
      this.y = planeSize + planeOffset -1;
    else
      this.y = newY;
  }

  // Draws this shape to a given context
  draw(ctx) {
    let oldColor = ctx.strokeStyle;
    let color = "0x000000";
    ctx.strokeStyle = color;
    ctx.fillStyle = color;
    ctx.lineWidth = siteWidth;
    ctx.beginPath();
    ctx.arc(this.x, this.y, siteRadius, 0, Math.PI * 2, true);
    ctx.stroke();
    ctx.strokeStyle = oldColor;
    ctx.fillStyle = oldColor;
  }
  // Determine if a point is inside the site's bounds
  contains(mx, my) {
    // used to make sure the Mouse X,Y fall in the circle of the site
    return dist(this.x, this.y, mx, my) < siteRadius + siteWidth;
  }
}

function sameSite(p1, p2) {
  return p1.id == p2.id;
}

function randomSite() {
  let offset = siteRadius*4;
  var p = new Site(planeOffset+1, planeOffset+1);
  p.x = randBetween(offset, planeSize-1-offset);
  p.y = randBetween(offset, planeSize-1-offset);
  return p;
}

//generates n sites
//no repeated coordinates
function randomSites(n) {
  var sites = []
  for (var i = 0; i < n; i++) {
    var repeated = true;
    var m = randomSite();
    while (repeated) {
      repeated = false;
      for (var j = 0; j < i; j++) {
        if (sites[j].x === m.x || sites[j].y === m.y) {
          repeated = true;
        }
      }
      if (repeated) m = randomSite();
    }
    sites.push(m);
  }
  return sites;
}

class CanvasState {
  constructor(canvas) {
    this.P = [];
    this.sites = [];
    this.appetite = 0;

    // setup
    this.canvas = canvas;
    this.width = canvas.width;
    this.height = canvas.height;
    this.ctx = canvas.getContext('2d');
    // This complicates things a little but fixes mouse co-ordinate problems
    // when there's a border or padding. See getMouse for more detail
    var stylePaddingLeft, stylePaddingTop, styleBorderLeft, styleBorderTop;
    if (document.defaultView && document.defaultView.getComputedStyle) {
      this.stylePaddingLeft = parseInt(document.defaultView.getComputedStyle(canvas, null)['paddingLeft'], 10) || 0;
      this.stylePaddingTop = parseInt(document.defaultView.getComputedStyle(canvas, null)['paddingTop'], 10) || 0;
      this.styleBorderLeft = parseInt(document.defaultView.getComputedStyle(canvas, null)['borderLeftWidth'], 10) || 0;
      this.styleBorderTop = parseInt(document.defaultView.getComputedStyle(canvas, null)['borderTopWidth'], 10) || 0;
    }
    // Some pages have fixed-position bars (like the stumbleupon bar) at the top or left of the page
    // They will mess up mouse coordinates and this fixes that
    var html = document.body.parentNode;
    this.htmlTop = html.offsetTop;
    this.htmlLeft = html.offsetLeft;
    // **** Keep track of state ****
    this.valid = false; // when set to false, the canvas will redraw everything

    this.dragging = false; // Keep track of when we are dragging
    // the current selected object. In the future we could turn this into an array for multiple selection
    this.selection = null;
    this.dragoffx = 0; // See mousedown and mousemove events for explanation
    this.dragoffy = 0;
    var myState = this;

    this.interval = 30;
    setInterval(function () { myState.draw(); }, myState.interval);
    //fixes a problem where double clicking causes text to get selected on the canvas
    canvas.addEventListener('selectstart', function (e) { e.preventDefault(); return false; }, false);
    // Up, down, and move are for dragging
    canvas.addEventListener('mousedown', function (e) {
      var mouse = myState.getMouse(e);
      var mx = mouse.x;
      var my = mouse.y;
      var sites = myState.sites;
      var l = sites.length;
      for (var i = l - 1; i >= 0; i--) {
        if (sites[i].contains(mx, my)) {
          var mySel = sites[i];
          // Keep track of where in the object we clicked
          // so we can move it smoothly (see mousemove)
          myState.dragoffx = mx - mySel.x;
          myState.dragoffy = my - mySel.y;
          myState.dragging = true;
          myState.selection = mySel;
          myState.matching = null;
          myState.valid = false;
          return;
        }
      }
      // havent returned means we have failed to select anything.
      // If there was an object selected, we deselect it
      if (myState.selection) {
        myState.selection = null;
        myState.valid = false; // Need to clear the old selection border
      }
    }, true);
    canvas.addEventListener('mousemove', function (e) {
      if (myState.dragging) {
        var mouse = myState.getMouse(e);
        // We don't want to drag the object by its top-left corner, we want to drag it
        // from where we clicked. Thats why we saved the offset and use it here
        myState.selection.setX(mouse.x - myState.dragoffx);
        myState.selection.setY(mouse.y - myState.dragoffy);
        myState.valid = false; // Something's dragging so we must redraw
      }
    }, true);
    canvas.addEventListener('mouseup', function (e) {
      myState.dragging = false;
    }, true);
    // double click for making or deleting sites
    canvas.addEventListener('dblclick', function (e) {
      var mouse = myState.getMouse(e);
      var mx = mouse.x;
      var my = mouse.y;
      var sites = myState.sites;
      var l = sites.length;
      for (var i = 0; i < l; i++) {
        var site = sites[i];
        if (site.contains(mx, my)) {
          myState.removeSite(sites[i]);
          return;
        }
      }
      var p = new Site(mx, my);
      myState.sites.push(p);
      setNumSitesField(myState.sites.length);
      myState.valid = false;
    }, true);

    canvas.addEventListener('keydown', function (e) {
      if ((e.keyCode == 46 || e.keyCode == 8) && myState.selection !== null) {
        let p = myState.selection;
        let sites = myState.sites;
        myState.removeSite(p);
      }
    }, true);
  }

  removeSite(site) {
    var found = false;
    var pos;
    for (var i = 0; i < this.sites.length; i++) {
      var p2 = this.sites[i];
      if (sameSite(site, p2)) {
        found = true;
        pos = i;
        break;
      }
    }
    if (found)
      this.sites.splice(pos, 1);
    this.valid = false;
    this.selection = null;
    setNumSitesField(this.sites.length);
  }

  setSites(sites) {
    this.sites = sites;
    this.dragging = false;
    this.selection = null;
    this.dragoffx = 0;
    this.dragoffy = 0;
    this.valid = false;
    setNumSitesField(sites.length);
  }

  setAppetite(app) {
    let appUnit = (planeSize*planeSize)/totalAppetite;
    this.appetite = app*appUnit;
    this.valid = false;
    setAppetiteField(app);
  }

  setMetric(met) {
    this.metric = met;
    this.P = pointsSorted(planeSize, met);
    this.valid = false;
  }
  

  clear() {
    this.ctx.clearRect(0, 0, this.width, this.height);
  }

  // While draw is called as often as the INTERVAL variable demands,
  // It only ever does something if the canvas gets invalidated by our code
  draw() {
    if (this.valid)
      return;
    var ctx = this.ctx;
    let oldColor = ctx.strokeStyle;
    var sites = this.sites;
    this.clear();
    // ** Add stuff you want drawn in the background all the time here **
    // draw all sites
    plane = assignRegions(sites, this.P, planeSize, this.appetite);
    drawPlane(ctx, sites, plane);
    var l = sites.length;
    for (let i = 0; i < l; i++) {
      var site = sites[i];
      site.draw(ctx);
    }
    // draw selection
    // a stroke along the edge of the selected site
    if (this.selection != null) {
      ctx.strokeStyle = selectionColor;
      ctx.lineWidth = selectionWidth;
      var mySel = this.selection;
      ctx.beginPath();
      ctx.arc(mySel.x, mySel.y, siteRadius + 3, 0, Math.PI * 2, true);
      ctx.stroke();
      ctx.strokeStyle = oldColor;
    }
    // ** Add stuff you want drawn on top all the time here **
    ctx.strokeStyle = "black";
    ctx.lineWidth = 2;
    ctx.strokeRect(planeOffset, planeOffset, planeSize, planeSize);
    this.valid = true;
  }

  // Creates an object with x and y defined, set to the mouse position relative to the state's canvas
  // If you wanna be super-correct this can be tricky, we have to worry about padding and borders
  getMouse(e) {
    var element = this.canvas, offsetX = 0, offsetY = 0, mx, my;
    // Compute the total offset
    if (element.offsetParent !== undefined) {
      do {
        offsetX += element.offsetLeft;
        offsetY += element.offsetTop;
      } while ((element = element.offsetParent));
    }
    // Add padding and border style widths to offset
    // Also add the <html> offsets in case there's a position:fixed bar
    offsetX += this.stylePaddingLeft + this.styleBorderLeft + this.htmlLeft;
    offsetY += this.stylePaddingTop + this.styleBorderTop + this.htmlTop;
    mx = e.pageX - offsetX;
    my = e.pageY - offsetY;
    // We return a simple javascript object (a hash) with x and y defined
    return { x: mx, y: my };
  }
}

function drawPlane(ctx, sites, plane) {
  let n = planeSize;
  let oldColor = ctx.strokeStyle;
  let lastPId = -1;
  for (let i = 0; i < n; i++) {
    for (let j = 0; j < n; j++) {
      let pId = plane[i][j];
      if (pId != -1) {
        if (pId != lastPId) {
          let sId = sites[pId].id;
          ctx.strokeStyle = colors[sId%colors.length];
          lastPId = pId;
        }
        let jr = j+1;
        while (jr < n && plane[i][jr] == pId) {
          jr++;
        }
        ctx.strokeRect(planeOffset+i,planeOffset+j,1,jr-j);
        j = jr;
      }
    }
  }
  ctx.strokeStyle = oldColor;
}

////////////////////////////
//algorithm
////////////////////////////
function oriDist(x,y,met) {
  if (met == 0) { //euclidean
    return x*x+y*y; //squared distance good enough for sorting
  } else if (met == 1) { //manhattan
    return x+y;
  } else { //chess
    if (x>y) return x;
    return y;
  }
}

function pointsSorted(n, met) {
  //counting sort
  maxDist = (n-1)*(n-1)*2+1;
  var counts = [];
  for (let i = 0; i < maxDist; i++) {
    counts[i] = 0;
  }
  for (let i = 0; i < n; i++) {
      for (let j = 0; j <= i; j++) {
          let dis = oriDist(i,j,met);
          counts[dis]++;
      }
  }
  var total = 0;
  for (let i = 0; i < maxDist; i++) {
      let oldCount = counts[i];
      counts[i] = total;
      total += oldCount;
  }
  let res = [];
  for (let i = 0; i < n; i++) {
      for (let j = 0; j <= i; j++) {
          let dis = oriDist(i,j,met);
          res[counts[dis]] = [i, j];
          counts[dis]++;
      }
  }
  // console.dir(res);
  return res;
}

function assignRegions(sites, P, planeSize, appetite) {
  let n = planeSize;
  plane = [];
  for (let i = 0; i < n; i++) {
    plane[i] = [];
    for (let j = 0; j < n; j++) {
      plane[i][j] = -1;
    }
  }

  remIds = [];
  appetites = [];
  for (let i = 0; i < sites.length; i++) {
    remIds[i] = i;
    appetites[i] = appetite;
  }

  let PIndex = 0;
  let freeCells = n*n;
  while (PIndex < P.length && freeCells > 0) {
    let i = P[PIndex][0];
    let j = P[PIndex][1];
    let iTrans = [i, i, -i, -i, j, j, -j, -j];
    let jTrans = [j, -j, j, -j, i, -i, i, -i];
    for (let t = 0; t < 8; t++) {
      for (let k = 0; k < remIds.length; k++) {
        let cId = remIds[k];
        let si = iTrans[t] + sites[cId].x - planeOffset;
        let sj = jTrans[t] + sites[cId].y - planeOffset;
        if (si>=0 && sj>=0 && si<n && sj<n &&
                plane[si][sj] == -1 && appetites[cId]>0) {
          plane[si][sj] = cId;
          appetites[cId]--;
          freeCells--;
          if (appetites[cId] == 0) {
            remIds.splice(k, 1);
            if (remIds.length == 0) return plane;
          }
        }
      }
    }
    PIndex++;
  }
  return plane;
}


////////////////////////////////////
//HTML interaction
////////////////////////////////////

var CS = new CanvasState(document.getElementById('canvas1'));
init();

function init() {
  CS.setMetric(0);
  document.getElementById("euclidean").checked = true;
  CS.setSites(randomSites(initNumSites));
  CS.setAppetite(initAppetite);
}

function setNumSites() {
  var text1 = document.getElementById('numsitesfield').value;
  let n = parseInt(text1);
  if (n > 100) {
    alert('No more than 100 please');
  } else if (n < 2) {
    alert('At least 2 sites');
  } else {
    CS.setSites(randomSites(n));
  }
}

function setAppetite() {
  var text1 = document.getElementById('appetitefield').value;
  let app = parseInt(text1);
  if (app > 50) {
    alert('At most 50 please');
  } else if (app < 1) {
    alert('Appetite should be at least 1');
  } else {
    CS.setAppetite(app);
  }
}

function setMetric(name) {
  if (name == "euclidean"){
    document.getElementById("euclidean").checked = true;
    document.getElementById("manhattan").checked = false;
    document.getElementById("chess").checked = false;
    CS.setMetric(0);
  } else if (name == "manhattan"){
    document.getElementById("euclidean").checked = false;
    document.getElementById("manhattan").checked = true;
    document.getElementById("chess").checked = false;
    CS.setMetric(1);
  } else {
    document.getElementById("euclidean").checked = false;
    document.getElementById("manhattan").checked = false;
    document.getElementById("chess").checked = true;
    CS.setMetric(2);
  }
}

function setNumSitesField(n) {
  document.getElementById('numsitesfield').value = n;
}
function setAppetiteField(app) {
  document.getElementById('appetitefield').value = app;
}
