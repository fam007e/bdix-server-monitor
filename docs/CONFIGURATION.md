# Configuration Guide

BDIX Server Monitor uses a JSON file to define the servers it monitors. By default, it looks for `data/server.json`.

## File Structure

The configuration file requires a specific JSON structure with three top-level arrays: `ftp`, `tv`, and `others`.

### Schema

```json
{
  "ftp": [
    "string", // URL of FTP server 1
    "string"  // URL of FTP server 2
  ],
  "tv": [
    "string"  // URL of TV server
  ],
  "others": [
    "string"  // URL of other server
  ]
}
```

### Description

| Field    | Type            | Description |
| :---     | :---            | :---        |
| `ftp`    | `Array<String>` | List of FTP server URLs. Used when `--ftp` flag is active. |
| `tv`     | `Array<String>` | List of IP TV or streaming server URLs. Used when `--tv` flag is active. |
| `others` | `Array<String>` | List of generic HTTP/HTTPS servers. Used when `--others` flag is active. |

## Example Configuration

Save this as `data/server.json`:

```json
{
  "ftp": [
    "http://ftp.amigait.com",
    "http://ftp.dhakacom.net",
    "http://ftp.circleftp.net",
    "http://movie.bossbd.net"
  ],
  "tv": [
    "http://tv.bdix.app",
    "http://iptv.live.net.bd",
    "http://jagobd.com"
  ],
  "others": [
    "http://cache.google.com",
    "http://mirror.dhakacom.com",
    "https://speedtest.isoc.org.bd"
  ]
}
```

## Reloading Configuration

You can reload the configuration file without restarting the application if you are in **Interactive Mode**.

1.  Select option **8 (Reload config)** from the main menu.
2.  The application will re-read `data/server.json` and update the server lists immediately.
