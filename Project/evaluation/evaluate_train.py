from collections import defaultdict
import os
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Read spikes


base_path = "output"  # Carpeta donde están las subcarpetas con topologías

for topology_name in os.listdir(base_path):
    topology_dir = os.path.join(base_path, topology_name)
    if not os.path.isdir(topology_dir):
        continue

    file_path = os.path.join(topology_dir, "train.txt")
    if not os.path.isfile(file_path):
        print(f"Warning: train.txt file not found in {topology_dir}")
        continue

    print(f"Processing topology: {topology_name}")

    output_base = os.path.join("evaluation", topology_name)
    plot_dir = os.path.join(output_base, "plot")
    os.makedirs(plot_dir, exist_ok=True)

    spike_counts = defaultdict(lambda: defaultdict(int))
    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line: continue
            _, neuron, real_class = map(int, line.split(','))
            spike_counts[neuron][real_class] += 1

    # Extraction of neurons and clases
    all_neurons = sorted(spike_counts.keys())
    all_classes = sorted({cls for neuron in spike_counts for cls in spike_counts[neuron]})

    # Create DataFrame
    data = []
    for neuron in all_neurons:
        row = [spike_counts[neuron].get(cls, 0) for cls in all_classes]
        data.append(row)

    df = pd.DataFrame(data, index=all_neurons, columns=[f"Class {cls}" for cls in all_classes])
    df.index.name = "Neuron"

    if df.empty:
        print(f"No valid data {file_path}. Topology omited.")
        with open("evaluation/empty_topologys.log", "a") as log_file:
            log_file.write(f"{topology_name} does not contain valid data.\n")
        continue

    df.to_csv(os.path.join(plot_dir, "spike_count_matrix.csv"))

    # Heat Map
    plt.figure(figsize=(10, 8))
    sns.heatmap(df, annot=True, fmt="d", cmap="YlGnBu", linewidths=0.5)
    plt.title("Spikes per neuron and real class")
    plt.xlabel("Real Class")
    plt.ylabel("Neuron")
    plt.tight_layout()
    heatmap_path = os.path.join(plot_dir, "heatMap.png")
    plt.savefig(heatmap_path)
    plt.close()

    # Winner neuron and precision
    best_neurons = {}
    precision_data = []

    for cls_idx, cls in enumerate(all_classes):
        col = df.iloc[:, cls_idx]
        total_spikes = col.sum()
        winner_neuron = col.idxmax()
        winner_count = col.max()
        precision = winner_count / total_spikes if total_spikes > 0 else 0.0
        best_neurons[cls] = (winner_neuron, winner_count, total_spikes, precision)
        precision_data.append([cls, winner_neuron, winner_count, total_spikes, round(precision, 4)])

    # Summary
    print("\nWinner neuron and class prediction")
    for cls, (neuron, count, total, prec) in best_neurons.items():
        print(f"Class {cls}: Neuron {neuron} with {count}/{total} spikes → Precision: {prec:.2%}")

    # Save results
    precision_df = pd.DataFrame(precision_data, columns=["Class", "Winner neuron", "Winner spike", "Total spikes", "Precision"])
    precision_df.set_index("Class", inplace=True)
    barplot_csv_path = os.path.join(plot_dir, "barPlot.csv")
    precision_df.to_csv(barplot_csv_path)

    # Precision bar diagram
    plt.figure(figsize=(10, 6))
    sns.barplot(data=precision_df.reset_index(), x="Class", y="Precision", color="skyblue")

    plt.ylim(0, 1)
    plt.title("Class precision (winner neuron)")
    plt.xlabel("Class")
    plt.ylabel("Precision")
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    barplot_png_path = os.path.join(plot_dir, "classPrecision.png")
    plt.savefig(barplot_png_path)
    plt.close()

    # Read data to build a temporal diagram
    raw_data = []

    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line: continue
            tiempo, neurona, clase_real = map(int, line.split(','))
            raw_data.append((tiempo, neurona, clase_real))

    df_time = pd.DataFrame(raw_data, columns=["time", "neuron", "real_class"])

    # Temporal activity diagram by class
    plt.figure(figsize=(12, 6))
    palette = sns.color_palette("tab10", n_colors=df_time['real_class'].nunique())

    sns.scatterplot(
        data=df_time,
        x="time",
        y="neuron",
        hue="real_class",
        palette=palette,
        s=20,
        alpha=0.7,
        linewidth=0
    )

    plt.title("Neural activity through time (colored by real class)")
    plt.xlabel("Spike time")
    plt.ylabel("Neuron index")
    plt.legend(title="Real class", bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.tight_layout()
    temporal_png_path = os.path.join(plot_dir, "temporalActivity.png")
    plt.savefig(temporal_png_path)
    plt.close()

    # Network predictions
    proy_prec_total = precision_df["Precision"].mean()
    print(f"Total precision: {proy_prec_total:.2%}\n")
