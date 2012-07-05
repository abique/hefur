// Author:  Alexandre Bique
// License: MIT

function parseByteSize(str) {
    str = str
        .replace("YiB", "*1024*1024*1024*1024*1024*1024*1024*1024")
        .replace("ZiB", "*1024*1024*1024*1024*1024*1024*1024")
        .replace("EiB", "*1024*1024*1024*1024*1024*1024")
        .replace("PiB", "*1024*1024*1024*1024*1024")
        .replace("TiB", "*1024*1024*1024*1024")
        .replace("GiB", "*1024*1024*1024")
        .replace("MiB", "*1024*1024")
        .replace("KiB", "*1024")
        .replace("B", "");
    return parseInt(eval(str));
}

function sortTbody2(tbody, col, lt) {
    rows = []
    for (i = 0; i < tbody.rows.length; ++i)
        rows.push(tbody.rows[i])
    rows = rows.sort(function (a, b) {
        return lt(a.cells[col].textContent, b.cells[col].textContent);
    });
    while (tbody.rows.length > 0)
        tbody.deleteRow(0);
    for (i in rows)
        tbody.appendChild(rows[i]);
    return rows;
}

function sortTbody(tbody_id, lt) {
    tbody = document.getElementById(tbody_id);
    col = this.event.srcElement.cellIndex;
    return sortTbody2(tbody, col, lt);
}

function sortStr(a, b) {
    return a > b;
}

function sortInt(a, b) {
    return parseInt(a) < parseInt(b);
}

function sortByte(a, b) {
    return parseByteSize(a) < parseByteSize(b);
}

function getSortMethod(elt) {
    for (i = 0; i < elt.classList.length; ++i)
        if (elt.classList.item(i))
            ;
}

function tutu() {
    msg = "col: " + this.event.srcElement.cellIndex;
    alert(msg);
}
