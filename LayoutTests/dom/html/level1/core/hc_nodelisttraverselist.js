
/*
Copyright Â© 2001-2004 World Wide Web Consortium, 
(Massachusetts Institute of Technology, European Research Consortium 
for Informatics and Mathematics, Keio University). All 
Rights Reserved. This work is distributed under the W3CÂ® Software License [1] in the 
hope that it will be useful, but WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

[1] http://www.w3.org/Consortium/Legal/2002/copyright-software-20021231
*/



   /**
    *  Gets URI that identifies the test.
    *  @return uri identifier of test
    */
function getTargetURI() {
      return "http://www.w3.org/2001/DOM-Test-Suite/level1/core/hc_nodelisttraverselist";
   }

var docsLoaded = -1000000;
var builder = null;

//
//   This function is called by the testing framework before
//      running the test suite.
//
//   If there are no configuration exceptions, asynchronous
//        document loading is started.  Otherwise, the status
//        is set to complete and the exception is immediately
//        raised when entering the body of the test.
//
function setUpPage() {
   setUpPageStatus = 'running';
   try {
     //
     //   creates test document builder, may throw exception
     //
     builder = createConfiguredBuilder();

      docsLoaded = 0;
      
      var docRef = null;
      if (typeof(this.doc) != 'undefined') {
        docRef = this.doc;
      }
      docsLoaded += preload(docRef, "doc", "hc_staff");
        
       if (docsLoaded == 1) {
          setUpPage = 'complete';
       }
    } catch(ex) {
    	catchInitializationError(builder, ex);
        setUpPage = 'complete';
    }
}



//
//   This method is called on the completion of 
//      each asychronous load started in setUpTests.
//
//   When every synchronous loaded document has completed,
//      the page status is changed which allows the
//      body of the test to be executed.
function loadComplete() {
    if (++docsLoaded == 1) {
        setUpPageStatus = 'complete';
    }
}


/**
* 
    The range of valid child node indices is 0 thru length -1
   
   Create a list of all the children elements of the third
   employee and traverse the list from index=0 thru
   length -1.     

* @author Curt Arnold
* @see http://www.w3.org/TR/1998/REC-DOM-Level-1-19981001/level-one-core#ID-203510337
* @see http://www.w3.org/TR/1998/REC-DOM-Level-1-19981001/level-one-core#ID-844377136
* @see http://www.w3.org/Bugs/Public/show_bug.cgi?id=246
*/
function hc_nodelisttraverselist() {
   var success;
    if(checkInitialization(builder, "hc_nodelisttraverselist") != null) return;
    var doc;
      var elementList;
      var employeeNode;
      var employeeList;
      var child;
      var childName;
      var nodeType;
      var result = new Array();

      expected = new Array();
      expected[0] = "em";
      expected[1] = "strong";
      expected[2] = "code";
      expected[3] = "sup";
      expected[4] = "var";
      expected[5] = "acronym";

      
      var docRef = null;
      if (typeof(this.doc) != 'undefined') {
        docRef = this.doc;
      }
      doc = load(docRef, "doc", "hc_staff");
      elementList = doc.getElementsByTagName("p");
      employeeNode = elementList.item(2);
      employeeList = employeeNode.childNodes;

      for(var indexN10073 = 0;indexN10073 < employeeList.length; indexN10073++) {
      child = employeeList.item(indexN10073);
      nodeType = child.nodeType;

      childName = child.nodeName;

      
	if(
	(1 == nodeType)
	) {
	result[result.length] = childName;

	}
	
		else {
			assertEquals("textNodeType",3,nodeType);
       assertEquals("textNodeName","#text",childName);
       
		}
	
	}
   assertEqualsListAutoCase("element", "nodeNames",expected,result);
       
}




function runTest() {
   hc_nodelisttraverselist();
}
