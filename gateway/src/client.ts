import path from "path";
import * as grpc from "@grpc/grpc-js";
import * as protoLoader from "@grpc/proto-loader";

// path to the proto file in the daemon folder
const PROTO_PATH = path.resolve(__dirname, "../../daemon/proto/openperf.proto");

// load proto
const packageDefinition = protoLoader.loadSync(PROTO_PATH, {
  keepCase: true,
  longs: String,
  enums: String,
  defaults: true,
  oneofs: true,
});

const protoDescriptor = grpc.loadPackageDefinition(
  packageDefinition
) as unknown as {
  openperf_rpc: {
    OpenPerfService: grpc.ServiceClientConstructor;
  };
};

const OpenPerfService = protoDescriptor.openperf_rpc.OpenPerfService;

// Types for our JSON API
export interface NodePayload {
  id?: string;
  tag: string;
  text?: string;
  role?: string;
  ariaLabel?: string;
  isInteractive?: boolean;
  children?: NodePayload[];
}

export interface PagePayload {
  id?: string;
  url: string;
  root: NodePayload;
}

// dreate a client instance
export function createOpenPerfClient(
  address: string = "localhost:50051"
): InstanceType<typeof OpenPerfService> {
  return new OpenPerfService(
    address,
    grpc.credentials.createInsecure()
  ) as any;
}