type MetricSample = { name: string; value: number; timestamp_unix_ms: string };

type Props = {
  metrics: MetricSample[];
};

const MetricsChart = ({ metrics }: Props) => {
  if (!metrics.length) {
    return <p>No metrics yet. Trigger a render pipeline to see data.</p>;
  }

  return (
    <div style={{ display: 'grid', gap: '0.5rem' }}>
      {metrics.map((m, idx) => (
        <div
          key={idx}
          style={{
            border: '1px solid #e0e0e0',
            borderRadius: '8px',
            padding: '0.75rem',
            display: 'flex',
            justifyContent: 'space-between',
            alignItems: 'center',
          }}
        >
          <div>
            <div style={{ fontWeight: 500 }}>{m.name}</div>
            <div style={{ fontSize: '0.8rem', color: '#777' }}>
              t={m.timestamp_unix_ms}
            </div>
          </div>
          <strong>{m.value.toFixed(3)}</strong>
        </div>
      ))}
    </div>
  );
};

export default MetricsChart;
