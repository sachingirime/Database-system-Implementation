# web_api.py
import asyncio
from fastapi import FastAPI, Request
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI()

# Enable CORS so frontend JS can connect
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

process = None

@app.on_event("startup")
async def start_cli():
    global process
    process = await asyncio.create_subprocess_exec(
        "./execs/main.out",
        stdin=asyncio.subprocess.PIPE,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE
    )

@app.post("/query")
async def run_query(request: Request):
    global process
    data = await request.json()
    query = data.get("query", "").strip()

    if not process:
        return JSONResponse({"error": "CLI process not running"}, status_code=500)

    # Write the query and flush
    process.stdin.write((query + "\n").encode())
    await process.stdin.drain()

    # Read output (we'll assume single response per query)
    await asyncio.sleep(0.3)  # allow CLI to output
    output = await process.stdout.readuntil(b'\n')
    
    return {"response": output.decode().strip()}

@app.get("/", response_class=HTMLResponse)
def get_terminal():
    return """
    <!DOCTYPE html>
    <html>
    <head>
        <title>MiniSQL Web Terminal</title>
        <style>
            body { background: black; color: white; font-family: monospace; padding: 20px; }
            input { width: 90%; padding: 10px; font-size: 1rem; }
            #output { white-space: pre-line; margin-top: 20px; }
        </style>
    </head>
    <body>
        <h2>🟢 MiniSQL Web Terminal</h2>
        <input id="query" placeholder="Enter SQL query..." />
        <div id="output"></div>
        <script>
            const input = document.getElementById("query");
            const output = document.getElementById("output");
            input.addEventListener("keydown", async function(e) {
                if (e.key === "Enter") {
                    const res = await fetch("/query", {
                        method: "POST",
                        headers: { "Content-Type": "application/json" },
                        body: JSON.stringify({ query: input.value })
                    });
                    const data = await res.json();
                    output.innerText += "> " + input.value + "\\n" + (data.response || data.error) + "\\n";
                    input.value = "";
                }
            });
        </script>
    </body>
    </html>
    """
