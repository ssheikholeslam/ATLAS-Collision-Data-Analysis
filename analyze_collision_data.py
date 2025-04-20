import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
import numpy as np
from scipy.stats import gaussian_kde
import os

# Verify that the input CSV exists
csv_path = "data/all_events.csv"
if not os.path.exists(csv_path):
    raise FileNotFoundError(f"Input CSV file not found at {csv_path}. Run the C++ CLI to generate all_events.csv.")

# Load the full dataset
try:
    data = pd.read_csv(csv_path)
except Exception as e:
    raise RuntimeError(f"Failed to load {csv_path}: {str(e)}")

# Verify expected columns
expected_columns = ['eventId', 'incomingParticles', 'outgoingParticles', 'kineticEnergyIn', 'restEnergyOut', 'efficiency']
missing_columns = [col for col in expected_columns if col not in data.columns]
if missing_columns:
    raise ValueError(f"Missing expected columns in {csv_path}: {missing_columns}")

# Trim whitespace from particle strings and handle NaN
data['incomingParticles'] = data['incomingParticles'].str.strip().fillna('')
data['outgoingParticles'] = data['outgoingParticles'].str.strip().fillna('')

# Function to count particles
def count_particles(particle_str, particle_type):
    if not particle_str or particle_str == '':
        return 0
    return particle_str.split(',').count(particle_type)

# Define particle types and extract counts
particle_types = ["electron", "muon", "photon", "jet", "tau"]
for p in particle_types:
    data[p + '_count'] = data['outgoingParticles'].apply(lambda x: count_particles(x, p))

# Calculate total particles per event
data['total_particles'] = data[[p + '_count' for p in particle_types]].sum(axis=1)

# 1. Efficiency Distribution with KDE
try:
    kde = gaussian_kde(data['efficiency'])
    x = np.linspace(data['efficiency'].min(), data['efficiency'].max(), 200)
    kde_y = kde(x) * len(data) * (data['efficiency'].max() - data['efficiency'].min()) / 50
    fig1 = go.Figure()
    fig1.add_trace(go.Histogram(x=data['efficiency'], nbinsx=50, name='Histogram',
                                marker_color='#1f77b4', opacity=0.7))
    fig1.add_trace(go.Scatter(x=x, y=kde_y, name='KDE', line=dict(color='#ff7f0e', width=2)))
    fig1.update_layout(
        title="Efficiency Distribution Across 100,000 Events",
        xaxis_title="Efficiency (Rest Energy Out / Total Energy In)",
        yaxis_title="Count",
        title_x=0.5,
        font=dict(size=14),
        legend=dict(x=0.8, y=0.9),
        barmode='overlay'
    )
    fig1.write_html("plots/efficiency_distribution.html")
except Exception as e:
    print(f"Warning: Failed to generate efficiency distribution plot: {str(e)}")

# 2. Particle Multiplicity Distributions
fig2 = go.Figure()
for p in particle_types:
    counts = data[p + '_count'].value_counts().sort_index()
    fig2.add_trace(go.Bar(
        x=counts.index,
        y=counts.values,
        name=p.capitalize(),
        marker_color=px.colors.qualitative.Plotly[particle_types.index(p)]
    ))
fig2.update_layout(
    barmode='group',
    title="Particle Multiplicity Across 100,000 Events",
    xaxis_title="Number of Particles per Event",
    yaxis_title="Number of Events",
    title_x=0.5,
    font=dict(size=14),
    legend=dict(orientation="h", yanchor="bottom", y=1.02, xanchor="center", x=0.5)
)
fig2.write_html("plots/particle_multiplicity.html")

# 3. Efficiency vs. Total Particle Count
fig3 = px.scatter(
    data,
    x='total_particles',
    y='efficiency',
    title="Efficiency vs. Total Particle Count",
    labels={'total_particles': 'Total Particles per Event', 'efficiency': 'Efficiency'},
    color='total_particles',
    color_continuous_scale='Viridis',
    hover_data=['eventId', 'outgoingParticles']
)
fig3.update_traces(marker=dict(size=6, opacity=0.6))
fig3.update_layout(title_x=0.5, font=dict(size=14))
fig3.write_html("plots/efficiency_vs_total_particles.html")

# 4. Particle Composition by Efficiency Decile
data['efficiency_decile'] = pd.qcut(data['efficiency'], 10, labels=False, duplicates='drop')
composition = data.groupby('efficiency_decile')[[p + '_count' for p in particle_types]].mean().reset_index()
fig4 = go.Figure()
for p in particle_types:
    fig4.add_trace(go.Bar(
        x=composition['efficiency_decile'],
        y=composition[p + '_count'],
        name=p.capitalize(),
        marker_color=px.colors.qualitative.Plotly[particle_types.index(p)]
    ))
fig4.update_layout(
    barmode='stack',
    title="Average Particle Composition by Efficiency Decile",
    xaxis_title="Efficiency Decile (0=Lowest, 9=Highest)",
    yaxis_title="Average Number of Particles",
    title_x=0.5,
    font=dict(size=14),
    legend=dict(orientation="h", yanchor="bottom", y=1.02, xanchor="center", x=0.5)
)
fig4.write_html("plots/particle_composition_by_efficiency.html")

# 5. Correlation Heatmap
corr_matrix = data[[p + '_count' for p in particle_types] + ['efficiency', 'total_particles']].corr()
fig5 = px.imshow(
    corr_matrix,
    text_auto=".2f",
    title="Correlation Between Particle Counts, Total Particles, and Efficiency",
    labels=dict(color="Correlation Coefficient"),
    color_continuous_scale="RdBu_r",
    zmin=-1,
    zmax=1
)
fig5.update_layout(title_x=0.5, font=dict(size=14), width=700, height=700)
fig5.write_html("plots/correlation_heatmap.html")

# 6. Conditional Probability of Particle Co-occurrence
co_occurrence = pd.DataFrame(index=particle_types, columns=particle_types)
for p1 in particle_types:
    for p2 in particle_types:
        if p1 != p2:
            p1_events = len(data[data[p1 + '_count'] > 0])
            if p1_events > 0:
                co_occurrence.loc[p1, p2] = len(data[(data[p1 + '_count'] > 0) & (data[p2 + '_count'] > 0)]) / p1_events
            else:
                co_occurrence.loc[p1, p2] = 0.0
        else:
            co_occurrence.loc[p1, p2] = 1.0
fig6 = px.imshow(
    co_occurrence.astype(float),
    text_auto=".2f",
    title="Conditional Probability of Particle Co-occurrence",
    labels=dict(color="P(p2|p1)"),
    color_continuous_scale="Blues",
    zmin=0,
    zmax=1
)
fig6.update_layout(title_x=0.5, font=dict(size=14), width=600, height=600)
fig6.write_html("plots/particle_co_occurrence.html")

print("Visualizations have been generated and saved to the 'plots' directory.")