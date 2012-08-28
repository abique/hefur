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

function sortTbody2(tbody, col, lt, reverse) {
    rows = []
    for (i = 0; i < tbody.rows.length; ++i)
        rows.push(tbody.rows[i])
    rows = rows.sort(function (a, b) {
        return lt(a.cells[col].textContent, b.cells[col].textContent);
    });
    if (reverse)
        rows = rows.reverse();
    while (tbody.rows.length > 0)
        tbody.deleteRow(0);
    for (i in rows)
        tbody.appendChild(rows[i]);
    return rows;
}

function sortTbody(tbody_id, lt) {
    src_el = this.event.srcElement;
    tbody = document.getElementById(tbody_id);
    col = src_el.cellIndex;

    reverse = false
    if (tbody.tableSorterCol == col)
        reverse = tbody.tableSorterReverse ^ true;

    tbody.tableSorterCol     = col;
    tbody.tableSorterReverse = reverse;

    return sortTbody2(tbody, col, lt, reverse);
}

function sortStr(a, b) {
    if (a < b)
        return 1;
    if (a == b)
        return 0;
    return -1;
}

function sortInt(a, b) {
    return parseInt(a) - parseInt(b);
}

function sortByte(a, b) {
    return parseByteSize(a) - parseByteSize(b);
}
