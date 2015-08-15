// This script saves the location details when a mouse button is
// clicked. This is for replacing data provided by QWebElement on a
// right-click if/when we switch to QT WebEngine which does not have
// an equivalent service.  This is always installed but only used if
// upplay is built with webengine (not the case and not possible for
// now)
var locDetails = '';
function saveLoc(ev) 
{
    el = ev.target;
    //console.log('el: ' + el);
    locDetails = '';
    while (el && !el.getAttribute('objid')) {
        //console.log('el: ' + el);
        el = el.parentNode;
    }
    if (el) {
        locdetails = '{\n' + 
            '"objid" : ' + JSON.stringify(el.getAttribute("objid")) + ',\n' +
            '"title" : ' + JSON.stringify(el.textContent || el.innerText || "") + ',\n' +
            '"objidx" : ' + JSON.stringify(el.getAttribute("objidx")) + ',\n' +
            '"otype" : ' + JSON.stringify(el.getAttribute("class")) + '\n' +
            '}\n';
    }
    //console.log(locdetails);
}
