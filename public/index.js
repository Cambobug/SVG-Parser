//Main functions

function initializeSvgViewTable()
{   
    //empties the select boxes
    $('#svgSelect').empty();
    $('#shapeSelect').empty();
    $('#scaleShape').empty();

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/svgFiles',
        data: {},
        success: function(data)
        {
            
            console.log("InitializedSvgView: successfully recieved data");
            if(data.fileData.length == 0)
            {
                $('#svgSelectLab').html("Select an SVG: No SVG's in uploads!");
            }
            else
            {
                data.fileData.forEach(fileName => {
                    if(getFileExt(fileName) === 'svg')
                    {
                        $.ajax({
                            async: false,
                            type: 'get',
                            dataType: 'json',
                            url: '/validateSVG',
                            data: {
                                file: fileName
                            },
                            success: function(data)
                            {
                                if(data.num == 1) // is a valid SVG file
                                {
                                    oneElem = true;
                                    let newSVGOp = new Option(fileName, fileName);
                                    let newShapeOp = new Option(fileName, fileName);
                                    let newScaleOp = new Option(fileName, fileName);
                                    $('#svgSelect').append(newSVGOp);
                                    $('#shapeSelect').append(newShapeOp);
                                    $('#scaleShape').append(newScaleOp);

                                }
                            },
                            error: function(error)
                            {
                                console.log('validateSVGFile: ' + error);
                            }
                        });
                    }
                });
                if(document.getElementById('svgSelect').options.length === 0)
                {
                    console.log('InitializedSvgView: No .svg files in /uploads!');
                    $('#svgSelectLab').html("Select an SVG: No SVG's in uploads!");
                }
                else
                {
                    console.log('InitializedSvgView: Found .svg files in /uploads!');
                    $('#svgSelectLab').html("Select an SVG:");
                    selectSVGToView();
                }
            }        
        },
        error: function(error)
        {
            console.log('InitializedSvgView: Failed to add to table: ' + error);
        }
    });
}

function refreshSvgTable()
{
    let oneElem = false;
    const svgPara = $('#svgTablePara');

    let numRows = document.getElementById('fullSVGTable').rows.length;

    while(numRows > 1)
    {
        document.getElementById('fullSVGTable').deleteRow(numRows - 1);
        numRows = document.getElementById('fullSVGTable').rows.length;
    }

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/svgFiles',
        data: {},
        success: function(files)
        {
            console.log("RefreshSvgTable: successfully recieved data");
            if(files.fileData.length == 0)
            {
                svgPara.html("No .svg files in /uploads!");
            }
            else
            {
                files.fileData.forEach(fileName => {
                    const extension = getFileExt(fileName);
                    if(extension === 'svg')
                    {
                        $.ajax({
                            async: false,
                            type: 'get',
                            dataType: 'json',
                            url: '/validateSVG',
                            data: {
                                file: fileName
                            },
                            success: function(validFiles)
                            {
                                if(validFiles.num == 1) // is a valid SVG file
                                {
                                    $.ajax({
                                        async: false,
                                        type: 'get',
                                        dataType: 'json',
                                        url: '/svgToJson',
                                        data: {
                                            validFile: fileName
                                        },
                                        success: function(data)
                                        {
                                            //console.log("/svgToJson: successfully recieved data, " + svgData.data);

                                            let values = JSON.parse(data.svgString);
                                            
                                            $('#svgTable').append("<tr style=\"height: 40px; text-align: center\"><td><a href=" + fileName + " download><img src=" + fileName + " id = \"svgTableImage\" \
                                            style=\"width: 200px;\">\ </a></td><td><a href=" + fileName + " download>" + fileName + "</a></td><td style=\"width: 10%;\">" + data.fileSize + "KB</td><td style=\"width: 10%;\" \
                                            >" + values.numRect + "</td><td style=\"width: 10%;\">" + values.numCirc + "</td><td style=\"width: 10%;\">" + values.numPaths + "</td><td style=\"width: 10%;\">\
                                            " + values.numGroups + "</td></tr>");
                                        },
                                        error: function(error)
                                        {
                                            console.log('/svgToJson: ' + error);
                                        }
                                    })
                                }
                            },
                            error: function(error)
                            {
                                console.log('validateSVGFile: ' + error);
                            }
                        });
                        oneElem = true;
                    }
                })

                if(oneElem == false)
                {
                    console.log("RefreshSvgTable: No .svg files in /uploads!");
                    svgPara.html("No .svg files in /uploads!");
                }
                else
                {
                    console.log("RefreshSvgTable: Found .svg files in /uploads");
                    svgPara.html("");
                }
            }
                
        },
        error: function(error)
        {
            console.log('RefreshSvgTable: Failed to add to table: ' + error);
        }
    });
}

function validateSVGFile(fileName)
{
    //console.log(fileName);
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/validateSVG',
        data: {
            file: fileName
        },
        success: function(data)
        {
            console.log("validateSVGFile: successfully recieved data, data.num = " + data.num);
            
            return data.num;
        },
        error: function(error)
        {
            console.log('validateSVGFile: ' + error);
        }
    });
}

function selectSVGToView()
{
    const currOp = $('#svgSelect').find(":selected").text();

    const img = document.getElementById('svgViewTableImg');

    $('#attrSelector').empty();

    img.src = currOp;
    img.alt = currOp + "image";
    
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/getSVGViewData',
        data: {
            file: currOp
        },
        success: function(data)
        {
            console.log("/getSVGViewData: successfully recieved data");

            let title = data.svgStrings[0];
            let desc = data.svgStrings[1];
            let rects = JSON.parse(data.svgStrings[2]);
            let circs = JSON.parse(data.svgStrings[3]);
            let paths = JSON.parse(data.svgStrings[4]);
            let groups = JSON.parse(data.svgStrings[5]);

            document.getElementById('svgTitleRow').innerHTML = title;
            document.getElementById('svgDescRow').innerHTML = desc;

            let numRows = document.getElementById('svgViewTable').rows.length;
            while(numRows > 4)
            {
                document.getElementById('svgViewTable').deleteRow(numRows - 1);
                numRows = document.getElementById('svgViewTable').rows.length;
            }

            let numAttrRows = document.getElementById('svgAttrViewTable').rows.length;
            while(numAttrRows > 1)
            {
                document.getElementById('svgAttrViewTable').deleteRow(numAttrRows - 1);
                numAttrRows = document.getElementById('svgAttrViewTable').rows.length;
            }
            
            for(let index = 0; index < rects.length; index++)
            {
                $('#svgViewTable').append("\
                <tr name=\"rectRow\"> \
                    <td colspan=\"1\">Rectangle " + (index + 1) + "</td>\
                    <td colspan=\"1\">Upper Left Corner: x: " + rects[index].x + rects[index].units + ", y: " + rects[index].y + rects[index].units + "<br>\
                    Width: " + rects[index].w + rects[index].units + ", Height: " + rects[index].h + rects[index].units + "</td>\
                    <td colspan=\"1\">" + rects[index].numAttr + "</td>\
                </tr>");

                $.ajax({
                    async: false,
                    type: 'get',
                    dataType: 'json',
                    url: '/getShapeAttrs',
                    data: {
                        file: currOp,
                        currIndex: index,
                        shape: 1
                    },
                    success: function(attrs)
                    {
                        console.log("Got attributes successfully");
                        let atts = JSON.parse(attrs.attrStr);

                        for(let attrIndex = 0; attrIndex < atts.length; attrIndex++)
                        {
                            $('#svgAttrViewTable').append("\
                            <tr name=\"rectRow\" onclick=\"displayAttrs()\"> \
                                <td>Rectangle " + (index + 1) + "\'s Attributes </td>\
                                <td>" + atts[attrIndex].name + ": " + atts[attrIndex].value + "</td><br>\
                            </tr>");

                            let newAttrOP = new Option("Rectangle " + (index + 1));
                            $('#attrSelector').append(newAttrOP);
                        }
                    },
                    error: function(error)
                    {
                        console.log('getShapeAttrs: ' + error);
                    }
                });
            }

            for(let index = 0; index < circs.length; index++)
            {
                $('#svgViewTable').append("\
                <tr name=\"circRow\"> \
                    <td colspan=\"1\">Circle " + (index + 1) + "</td>\
                    <td colspan=\"1\">Centre x: " + circs[index].cx + circs[index].units + ", y: " + circs[index].cy + circs[index].units + "\
                    , radius: " + circs[index].r + circs[index].units + "</td>\
                    <td colspan=\"1\">" + circs[index].numAttr + "</td>\
                </tr>");

                $.ajax({
                    async: false,
                    type: 'get',
                    dataType: 'json',
                    url: '/getShapeAttrs',
                    data: {
                        file: currOp,
                        currIndex: index,
                        shape: 2
                    },
                    success: function(attrs)
                    {
                        console.log("Got attributes successfully");
                        let atts = JSON.parse(attrs.attrStr);

                        for(let attrIndex = 0; attrIndex < atts.length; attrIndex++)
                        {
                            $('#svgAttrViewTable').append("\
                            <tr name=\"rectRow\"> \
                                <td>Circle " + (index + 1) + "\'s Attributes </td>\
                                <td>" + atts[attrIndex].name + ": " + atts[attrIndex].value + "</td><br>\
                            </tr>");

                            let newAttrOP = new Option("Circle " + (index + 1));
                            $('#attrSelector').append(newAttrOP);
                        }
                    },
                    error: function(error)
                    {
                        console.log('getShapeAttrs: ' + error);
                    }
                });
            }

            for(let index = 0; index < paths.length; index++)
            {
                $('#svgViewTable').append("\
                <tr name=\"pathRow\"> \
                    <td colspan=\"1\">Path " + (index + 1) + "</td>\
                    <td colspan=\"1\">Path Data: " + paths[index].d + "</td>\
                    <td colspan=\"1\">" + paths[index].numAttr + "</td>\
                </tr>");

                $.ajax({
                    async: false,
                    type: 'get',
                    dataType: 'json',
                    url: '/getShapeAttrs',
                    data: {
                        file: currOp,
                        currIndex: index,
                        shape: 3
                    },
                    success: function(attrs)
                    {
                        console.log("Got attributes successfully");
                        let atts = JSON.parse(attrs.attrStr);

                        for(let attrIndex = 0; attrIndex < atts.length; attrIndex++)
                        {
                            $('#svgAttrViewTable').append("\
                            <tr name=\"rectRow\"> \
                                <td>Path " + (index + 1) + "\'s Attributes </td>\
                                <td>" + atts[attrIndex].name + ": " + atts[attrIndex].value + "</td><br>\
                            </tr>");

                            let newAttrOP = new Option("Path " + (index + 1));
                            $('#attrSelector').append(newAttrOP);
                        }
                    },
                    error: function(error)
                    {
                        console.log('getShapeAttrs: ' + error);
                    }
                });
            }

            for(let index = 0; index < groups.length; index++)
            {
                $('#svgViewTable').append("\
                <tr name=\"groupRow\"> \
                    <td colspan=\"1\">Group " + (index + 1) + "</td>\
                    <td colspan=\"1\">" + groups[index].children + " child elements</td>\
                    <td colspan=\"1\">" + groups[index].numAttr + "</td>\
                </tr>");

                $.ajax({
                    async: false,
                    type: 'get',
                    dataType: 'json',
                    url: '/getShapeAttrs',
                    data: {
                        file: currOp,
                        currIndex: index,
                        shape: 4
                    },
                    success: function(attrs)
                    {
                        console.log("Got attributes successfully");
                        let atts = JSON.parse(attrs.attrStr);

                        for(let attrIndex = 0; attrIndex < atts.length; attrIndex++)
                        {
                            $('#svgAttrViewTable').append("\
                            <tr name=\"rectRow\"> \
                                <td>Group" + (index + 1) + "\'s Attributes </td>\
                                <td>" + atts[attrIndex].name + ": " + atts[attrIndex].value + "</td><br>\
                            </tr>");

                            let newAttrOP = new Option("Group " + (index + 1));
                            $('#attrSelector').append(newAttrOP);
                        }
                    },
                    error: function(error)
                    {
                        console.log('getShapeAttrs: ' + error);
                    }
                });
            }
        },
        error: function(error)
        {
            console.log('/seclectSVGToView: ' + error);
        }
    }); 
}

//Helper Functions/

function getFileExt(fileName)
{
    if(fileName === "svg")
    {
        return "err";
    }
    // code line found at "https://stackoverflow.com/questions/190852/how-can-i-get-file-extensions-with-javascript/1203361#1203361"
    return fileName.substring(fileName.lastIndexOf('.')+1, fileName.length) || fileName; 
}

// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {

    console.log("Ready: On page load, successfully received data");
    /*
    let validSVGs = getValidSVGFiles();
    console.log(validSVGs);
    */
    initializeSvgViewTable(); // initializes the individual SVG view table+

    refreshSvgTable();

    console.log('Ready: Populated web page and tables with initial values');

    /**********Event Handlers ************/

    let svgSelector = document.getElementById("svgSelect");
    svgSelector.addEventListener("change", function()
    {
        selectSVGToView();
    });

    let attrButton = document.getElementById("editAttrBut");
    attrButton.onclick = function()
    {
        console.log("Attribute Submission Stub: attribute submitted");
    };
    
    $("#editTitleDesc").submit(function(e){
        const titleStr = $('#updateTitleInput').val();
        const descStr = $('#updateDescInput').val();

        const fileName = $('#svgSelect').val();

        e.preventDefault();

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/updateTitleAndDesc',
            data:{
                file: fileName,
                title: titleStr,
                desc: descStr
            },
            success: function(data)
            {
                console.log('/updateditTitleDesce: successfully recieved data');
                if(data.num == 1)
                {
                    console.log('/updateditTitleDesce: successfully updated title and desc');
                    location.reload();
                }
                else
                {
                    console.log('/updateditTitleDesce: failed to update title and desc');
                }
            },
            error: function (error)
            {
                console.log('/updateditTitleDesce: ' + error);
            }

        });
    });


    $("#createSVGForm").submit(function(e){
        const titleStr = $('#titleInput').val();
        const descStr = $('#descInput').val();

        const fileName = $('#fileInput').val();

        let select = document.getElementById('svgSelect');
        let numOptions = select.length;
        let notUnique = 0;
        for(let i = 0; i < numOptions; i++)
        {
            if(select.options[i].text === fileName)
            {
                notUnique = 1;
            }
        }

        e.preventDefault();

        const ext = getFileExt(fileName);

        document.getElementById("createSVGErrLab").innerText = "";
        if(ext === "svg" && notUnique === 0)
        {   
            $.ajax({
                async: false,
                type: 'get',
                dataType: 'json',
                url: '/createSVG',
                data: {
                    file: fileName,
                    title: titleStr,
                    desc: descStr
                },
                success: function(data)
                {
                    console.log('/createSVG: successfully recieved data');
                    if(data.num == 1)
                    {
                        console.log('/createSVG: successfully created SVG');
                        location.reload();
                    }
                    else
                    {
                        alert("Failed to create an SVG with given arguments");
                        console.log('/createSVG: failed to create SVG');
                    }
                },
                error: function(error)
                {
                    alert("Failed to create an SVG with given arguments");
                    console.log('/createSVG: ' + error);
                }
            });
        }
        else
        {
            alert("Failed to create an SVG with given arguments");
            console.log("filename must be an svg file");
            document.getElementById("createSVGErrLab").innerText = "File input field must be a .svg file!";
        }
    });


    $("#createRectForm").submit(function(e){
        let formX = $('#xInput').val();
        let formY = $('#yInput').val();
        let formW = $('#wInput').val();
        let formH = $('#hInput').val();
        const fill = $('#rectColorpicker').val();

        const fileName = $('#shapeSelect').find(":selected").text();

        e.preventDefault();

        document.getElementById("createRectErrLab").innerText = "";
        if(isNaN(formX) || isNaN(formY) || isNaN(formW) || isNaN(formH))
        {
            alert("/createRectForm: Form input must be a valid number");
            console.log("/createRectForm: Form input must be a valid number");
            document.getElementById("createRectErrLab").innerText = "Input fields must have valid numbers! (num >= 0)";
        }
        else if(formX < 0 || formY < 0 || formW < 0 || formH < 0)
        {
            alert("/createRectForm: Form input must be a valid number");
            console.log("/createRectForm: Form input must be a valid number");
            document.getElementById("createRectErrLab").innerText = "Input fields must have valid numbers! (num >= 0)";
        }
        else if(formX.length == 0 || formY.length == 0 || formW.length == 0 || formH.length == 0)
        {
            alert("/createRectForm: Form input must be a valid number");
            console.log("/createRectForm: Form input must be a valid number");
            document.getElementById("createRectErrLab").innerText = "Input fields must have valid numbers! (num >= 0)";
        }
        else
        {
            $.ajax({
                async: false,
                type: 'get',
                dataType: 'json',
                url: '/createRect',
                data: {
                    x: formX,
                    y: formY,
                    w: formW,
                    h: formH,
                    colour: fill,
                    file: fileName
                },
                success: function(data)
                {
                    console.log('/createRect: got data');
                    if(data.num == 1)
                    {
                        console.log('/createRect: created rect successfully');
                        location.reload(); // reloads page to update the tables
                    }
                    else
                    {
                        alert('/createRect: failed to create rect');
                        console.log('/createRect: failed to create rect');
                    }
                },
                error: function(error)
                {
                    alert('/createRect: ' + error);
                    console.log('/createRect: ' + error);
                }
            });
        }
    });
    

    $("#createCircForm").submit(function(e){
        let formX = $('#cxInput').val();
        let formY = $('#cyInput').val();
        let formR = $('#rInput').val();
        const fill = $('#circColorpicker').val();

        const fileName = $('#shapeSelect').find(":selected").text();

        e.preventDefault();

        document.getElementById("createCircErrLab").innerText = "";
        if(isNaN(formX) || isNaN(formY) || isNaN(formR))
        {
            alert("/createCircForm: Form input must be a valid number");
            console.log("/createCircForm: Form input must be a valid number");
            document.getElementById("createCircErrLab").innerText = "Input fields must have valid numbers! (num >= 0)";
        }
        else if(formX < 0 || formY < 0 || formR < 0)
        {
            alert("/createCircForm: Form input must be a valid number");
            console.log("/createCircForm: Form input must be a valid number");
            document.getElementById("createCircErrLab").innerText = "Input fields must have valid numbers! (num >= 0)";
        }
        else if(formX.length == 0 || formY.length == 0 || formR.length == 0)
        {
            alert("/createCircForm: Form input must be a valid number");
            console.log("/createCircForm: Form input must be a valid number");
            document.getElementById("createCircErrLab").innerText = "Input fields must have valid numbers! (num >= 0)";
        }
        else
        {
            $.ajax({
                async: false,
                type: 'get',
                dataType: 'json',
                url: '/createCircle',
                data: {
                    cx: formX,
                    cy: formY,
                    radius: formR,
                    colour: fill,
                    file: fileName
                },
                success: function(data)
                {
                    console.log('/createCircle: got data');
                    if(data.num == 1)
                    {
                        console.log('/createCircle: created circle successfully');
                        location.reload(); // reloads page to update the tables
                        //MAKE IT SO SELECTS REMAIN AS THEY WERE BEFORE PAGE REFRESH
                    }
                    else
                    {
                        alert('/createCircle: failed to create circle');
                        console.log('/createCircle: failed to create circle');
                    }
                },
                error: function(error)
                {
                    alert('/createCircle: ' + error);
                    console.log('/createCircle: ' + error);
                }
            });
        }
    });
    
    // submits the scale shape form and scales the selected shapes in the selected file
    $("#scaleShapeForm").submit(function(e){
        let formVal = $('#scaleChngInput').val(); // gets the value of the scale change
        const fileName = $('#scaleShape').find(":selected").text(); // gets the selected file
        let radioButCheck = 0;

        e.preventDefault();

        if($('#scaleRectRadBut').is(':checked')) // if the circle button is selected
        {
            console.log("Scaling Shape: rects");
            radioButCheck = 0;
        }
        if($('#scaleCircRadBut').is(':checked')) // if the rect button is selected
        {
            console.log("Scaling Shape: circ");
            radioButCheck = 1;
        }

        document.getElementById("scaleChngErrLab").innerText = "";
        if((isNaN(formVal)) || (formVal < 0) || (formVal.length == 0))
        {
            alert('/scaleChange: invalid arguments');
            console.log("Form input must be a valid number");
            document.getElementById("scaleChngErrLab").innerText = "Input must be a valid number (num >= 0)!";
        }
        else
        {
            $.ajax({
                async: false,
                type: 'get',
                dataType: 'json',
                url: '/scaleShapes',
                data: {
                    file: fileName,
                    scale: formVal,
                    shape: radioButCheck
                },
                success: function(data)
                {   
                    console.log('/scaleChange: got data');
                    if(data.num == 1)
                    {
                        console.log('/scaleChange: change shapes successfully');
                        location.reload(); // reloads page to update the tables
                    }
                    else
                    {
                        alert('/scaleChange: failed to change shapes');
                        console.log('/scaleChange: failed to change shapes');
                    }
                },
                error: function(error)
                {
                    alert('/scaleChange: ' + error);
                    console.log('/scaleChange: ' + error);
                }
            });
        } 
    });
    
});