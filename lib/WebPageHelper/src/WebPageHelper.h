#ifndef _WebPageHelper_h
#define _WebPageHelper_h

#include <Arduino.h>

static String getTableRow(String key, String value)
{
    String ret = String("<tr>");
    ret += String(" <td>");
    ret += String("     <strong>");
    ret += key;
    ret += String("     </strong>");
    ret += String(" </td>");
    ret += String(" <td>");
    ret += value;
    ret += String(" </td>");
    ret += String("</tr>");
    return ret;
}

// ----------------------------------------------------------------------

static String getPageHeader(String uriTitle, String uri)
{
    String ret = String("<!DOCTYPE html>");
    ret += String("<html>");
    ret += String("    <head>");
    ret += String("        <title>ESP32 - GPS Logger</title>");
    ret += String("        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    ret += String("        <style>");
    ret += String("            body {");
    ret += String("                font-family: Helvetica, Arial, sans-serif;");
    ret += String("            }");
    ret += String("            table {");
    ret += String("                border: 1px solid black;");
    ret += String("                border-collapse: collapse;");
    ret += String("                width: 100%;");
    ret += String("            }");
    ret += String("            .footer {");
    ret += String("                bottom: 10px;");
    ret += String("                position: absolute;");
    ret += String("            }");
    ret += String("        </style>");
    ret += String("    </head>");
    ret += String("<div><a href=\""+ uri +"\">" + uriTitle + "</a></div");

    return ret;
}

// ----------------------------------------------------------------------

static String getPageBody(String h1, String content)
{
    String ret = String("    <body>");
    ret += String("<h1>" + h1 + "</h1>");
    ret += content;
    ret += String("        <div class=\"footer\"><small><a href=\"mailto:paddel10@hotmail.com\">paddel10@hotmail.com</a> &copy; 06/2020</small></div>");
    ret += String("    </body>");
    return ret;
}

// ----------------------------------------------------------------------

static String getPageFooter()
{
    String ret = String("</html>");
    return ret;
}

#endif
