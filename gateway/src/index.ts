import express from "express";
import * as grpc from "@grpc/grpc-js";
import { createOpenPerfClient, PagePayload } from "./client";

const PORT = process.env.PORT || 3000;
const GRPC_ADDRESS = process.env.OPENPERF_GRPC_ADDRESS || "localhost:50051";

const app = express();
app.use(express.json());

const client = createOpenPerfClient(GRPC_ADDRESS);

// convert JSON NodePayload into proto shape
function toProtoNode(node: any): any {
  return {
    id: node.id ?? "",
    tag: node.tag ?? "",
    text: node.text ?? "",
    role: node.role ?? "",
    aria_label: node.ariaLabel ?? "",
    is_interactive: node.isInteractive ?? false,
    children: (node.children ?? []).map(toProtoNode),
  };
}

// POST /pages -> SubmitPage
app.post("/pages", (req, res) => {
  const body: PagePayload = req.body;

  if (!body || !body.url || !body.root) {
    return res.status(400).json({ error: "Missing url or root in body" });
  }

  const pageMessage = {
    id: body.id ?? "",
    url: body.url,
    root: toProtoNode(body.root),
  };

  client.SubmitPage(
    { page: pageMessage },
    (err: grpc.ServiceError | null, response: any) => {
      if (err) {
        console.error("SubmitPage error:", err);
        return res.status(500).json({ error: err.message });
      }
      if (!response) {
        console.error("SubmitPage: received null/undefined response");
        return res.status(500).json({ error: "Invalid response from daemon" });
      }
      console.log("SubmitPage response:", JSON.stringify(response, null, 2));
      console.log("SubmitPage response keys:", Object.keys(response || {}));
      // Try both snake_case and camelCase field names
      const pageId = response.page_id || response.pageId || "";
      if (!pageId) {
        console.error("SubmitPage: page_id is empty in response", response);
        return res.status(500).json({ error: "Daemon returned empty page ID" });
      }
      return res.json({ pageId });
    }
  );
});

// POST /pages/:id/render -> RunRenderPipeline
app.post("/pages/:id/render", (req, res) => {
  const pageId = req.params.id;
  client.RunRenderPipeline(
    { page_id: pageId },
    (err: grpc.ServiceError | null, response: any) => {
      if (err) {
        console.error("RunRenderPipeline error:", err);
        return res.status(500).json({ error: err.message });
      }
      return res.json({ status: "ok" });
    }
  );
});

// GET /pages/:id/a11y -> AnalyzeAccessibility
app.get("/pages/:id/a11y", (req, res) => {
  const pageId = req.params.id;
  client.AnalyzeAccessibility(
    { page_id: pageId },
    (err: grpc.ServiceError | null, response: any) => {
      if (err) {
        console.error("AnalyzeAccessibility error:", err);
        return res.status(500).json({ error: err.message });
      }
      return res.json({ issues: response.issues });
    }
  );
});

// GET /metrics -> GetMetrics
app.get("/metrics", (_req, res) => {
  client.GetMetrics({}, (err: grpc.ServiceError | null, response: any) => {
    if (err) {
      console.error("GetMetrics error:", err);
      return res.status(500).json({ error: err.message });
    }
    return res.json({ samples: response.samples });
  });
});

app.listen(PORT, () => {
  console.log(
    `OpenPerf gateway listening on http://localhost:${PORT}, talking to gRPC at ${GRPC_ADDRESS}`
  );
});
