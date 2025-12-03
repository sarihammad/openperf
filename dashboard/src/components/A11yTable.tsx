type A11yIssue = { element: string; description: string; severity: number };

type Props = {
  issues: A11yIssue[];
};

const severityLabel = (level: number) => {
  if (level >= 3) return "High";
  if (level === 2) return "Medium";
  return "Low";
};

const A11yTable = ({ issues }: Props) => {
  if (issues.length === 0) {
    return <p>No accessibility issues detected.</p>;
  }

  return (
    <table style={{ width: "100%", borderCollapse: "collapse" }}>
      <thead>
        <tr>
          <th style={{ textAlign: "left", padding: "0.5rem", borderBottom: "1px solid #ccc" }}>
            Element
          </th>
          <th style={{ textAlign: "left", padding: "0.5rem", borderBottom: "1px solid #ccc" }}>
            Description
          </th>
          <th style={{ textAlign: "left", padding: "0.5rem", borderBottom: "1px solid #ccc" }}>
            Severity
          </th>
        </tr>
      </thead>
      <tbody>
        {issues.map((issue) => (
          <tr key={`${issue.element}-${issue.description}`}>
            <td style={{ padding: "0.5rem", borderBottom: "1px solid #eee" }}>{issue.element}</td>
            <td style={{ padding: "0.5rem", borderBottom: "1px solid #eee" }}>{issue.description}</td>
            <td style={{ padding: "0.5rem", borderBottom: "1px solid #eee" }}>
              {severityLabel(issue.severity)}
            </td>
          </tr>
        ))}
      </tbody>
    </table>
  );
};

export default A11yTable;
