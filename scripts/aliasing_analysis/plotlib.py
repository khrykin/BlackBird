def apply_sliders(fig, lines_num, labels, prefix="Prefix: ", suffix=" suffix"):
    active_ind = 1

    for k in range(lines_num):
        fig.data[lines_num*active_ind+k].visible = True

    steps = []
    for i in range(len(fig.data)//lines_num):
        step = dict(
            method="update",
            args=[{"visible": [False] * len(fig.data)},],
            label=str(labels[i])
        )

        for k in range(lines_num):
            step["args"][0]["visible"][lines_num*i+k] = True

        steps.append(step)

    sliders = [dict(
        active=active_ind,
        currentvalue={"prefix": prefix, "suffix": suffix},
        pad={"t": 50},
        steps=steps
    )]

    fig.update_layout(sliders=sliders, hovermode="x unified")

    return fig
