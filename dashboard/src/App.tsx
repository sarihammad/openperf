import { useEffect, useState } from 'react';
import MetricsChart from './components/MetricsChart';
import A11yTable from './components/A11yTable';

type MetricSample = {
  name: string;
  value: number;
  timestamp_unix_ms: string; // comes back as a string from gateway
};

type A11yIssue = {
  code: string;
  message: string;
  severity: string;
  node_id: string;
};

function App() {
  const [metrics, setMetrics] = useState<MetricSample[]>([]);
  const [issues, setIssues] = useState<A11yIssue[]>([]);
  const [pageId, setPageId] = useState<string>('page-0');
  const [loadingIssues, setLoadingIssues] = useState(false);
  const [submittingPage, setSubmittingPage] = useState(false);

  // Poll /metrics every 3s
  useEffect(() => {
    async function fetchMetrics() {
      try {
        const res = await fetch('http://localhost:3000/metrics');
        const json = await res.json();
        setMetrics(json.samples ?? []);
      } catch (e) {
        console.error('Failed to fetch metrics', e);
      }
    }

    fetchMetrics();
    const id = setInterval(fetchMetrics, 3000);
    return () => clearInterval(id);
  }, []);

  // Fetch issues for current page
  async function loadIssues() {
    if (!pageId) return;
    setLoadingIssues(true);
    try {
      const res = await fetch(`http://localhost:3000/pages/${pageId}/a11y`);
      const json = await res.json();
      setIssues(json.issues ?? []);
    } catch (e) {
      console.error('Failed to fetch issues', e);
      setIssues([]);
    } finally {
      setLoadingIssues(false);
    }
  }

  // Submit a sample page (similar to your curl)
  async function submitSamplePage() {
    setSubmittingPage(true);
    try {
      const res = await fetch('http://localhost:3000/pages', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          url: 'https://example.com',
          root: {
            tag: 'div',
            id: 'root',
            children: [{ tag: 'img', id: 'hero-image' }],
          },
        }),
      });
      const json = await res.json();
      if (json.pageId) {
        setPageId(json.pageId);
      }
    } catch (e) {
      console.error('Failed to submit sample page', e);
    } finally {
      setSubmittingPage(false);
    }
  }

  return (
    <main
      style={{
        maxWidth: 960,
        margin: '2rem auto',
        padding: '0 1rem',
        fontFamily: 'Inter, system-ui, sans-serif',
      }}
    >
      <header style={{ marginBottom: '1.5rem' }}>
        <h1>OpenPerf Dashboard</h1>
        <p style={{ color: '#555' }}>
          Render pipeline metrics and accessibility issues from the OpenPerf
          engine.
        </p>
      </header>

      <section style={{ marginBottom: '2rem' }}>
        <h2>Metrics</h2>
        <MetricsChart metrics={metrics} />
      </section>

      <section style={{ marginBottom: '2rem' }}>
        <h2>Accessibility</h2>

        <div
          style={{
            display: 'flex',
            gap: '0.75rem',
            alignItems: 'center',
            marginBottom: '0.75rem',
          }}
        >
          <label>
            Page ID:&nbsp;
            <input
              value={pageId}
              onChange={(e) => setPageId(e.target.value)}
              style={{ padding: '0.25rem 0.5rem' }}
            />
          </label>
          <button onClick={loadIssues} disabled={loadingIssues || !pageId}>
            {loadingIssues ? 'Loading…' : 'Load issues'}
          </button>
          <button onClick={submitSamplePage} disabled={submittingPage}>
            {submittingPage ? 'Submitting…' : 'Submit sample page'}
          </button>
        </div>

        <A11yTable
          issues={issues.map((issue) => ({
            element: issue.node_id,
            description: issue.message,
            severity:
              issue.severity === 'SEVERITY_ERROR'
                ? 3
                : issue.severity === 'SEVERITY_WARNING'
                ? 2
                : 1,
          }))}
        />
      </section>
    </main>
  );
}

export default App;
