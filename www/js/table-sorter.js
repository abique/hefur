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
    return eval(str);
}

function sortTbody(tbody, col, lt) {
    rows = []
    for (i = 0; i < tbody.rows.length; ++i)
        rows.push(tbody.rows[i]);
    rows = rows.sort(function (a, b) {
        return lt(a.cells[col].textContent, b.cells[col].textContent);
        });
    while (tbody.rows.length > 0)
        tbody.deleteRow(0);
    for (i in rows)
        tbody.appendChild(rows[i]);
    return rows;
}
